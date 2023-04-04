#include "shader_lib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cglm/include/cglm/cglm.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#define FBDEV "/dev/fb0"

#define SCR_WIDTH  800
#define SCR_HEIGHT  600
struct _FrameBuffer;
typedef struct _FrameBuffer FrameBuffer;

typedef unsigned char BYTE;

typedef struct{
    void* data;
    void* next;
}No;

typedef struct{
    No *head;
    No *tail;
}LinkedList;

typedef struct{
    unsigned int TextureID;  // ID handle of the glyph texture
    vec2 Size;       // Size of glyph
    vec2 Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
}Character;

struct _FrameBuffer
  {
  int fd; // File descriptor
  int w; // Displayed width in pixels
  int h; // Displayer height in pixels
  int fb_data_size; // Total amount of mapped memory
  BYTE *fb_data; // Pointer to the mapped memory
  char *fbdev; // Original device name
  int fb_bytes; // Number of bytes per pixel -- must by 3 or 4
  int line_length; // Number of pixels in a line, as reported by the device
  int stride; // Bytes between vertically-adjacent rows of pixels
  int slop; // Amount of line_length that does not correspond to pixels.
};

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
LinkedList* makeLinkedList();
No* makeNo(void* pointer2Struct);
Character* makeCharacter(unsigned int texture,FT_Face face);
int addCharacter(LinkedList *linkedList, unsigned int texture, FT_Face face);
FrameBuffer     *framebuffer_create (const char *fbdev);
void             framebuffer_set_pixel (FrameBuffer *self, int x,
                      int y, BYTE r, BYTE g, BYTE b);

unsigned int VAOText, VBOText;
LinkedList *characterLinkedList;

int main(){
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL){
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    // OpenGL state
    // ------------
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shader program (text)
    // ------------------------------------
    GLuint text_vertex = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(&text_vertex, GL_VERTEX_SHADER, "text.vs");

    GLuint text_frag = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(&text_frag, GL_FRAGMENT_SHADER, "text.fs");

    GLuint textShaderProgram = glCreateProgram();
    link_shader(text_vertex, text_frag, textShaderProgram);

    // FreeType
    // --------
    FT_Library ft;
    if (FT_Init_FreeType(&ft)){
        fprintf(stderr, "ERROR-FREETYPE: Could not init FreeType Library \n");
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/crosextra/Caladea-Regular.ttf", 0, &face)){
        fprintf(stderr, "ERROR-FREETYPE: Failed to load font \n");  
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    FT_UInt gi = FT_Get_Char_Index (face, 'x');
    if (!gi){
        fprintf(stderr, "ERROR-FREETYTPE: Failed to get a glyph \n");  
        return -1;
    }
    FT_Load_Glyph (face, gi, FT_LOAD_DEFAULT);
    int bbox_ymax = face->bbox.yMax / 64;
    int glyph_width = face->glyph->metrics.width / 64;
    int advance = face->glyph->metrics.horiAdvance / 64;
    int x_off = (advance - glyph_width) / 2;
    int y_off = bbox_ymax - face->glyph->metrics.horiBearingY / 64;
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

    /*
       if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)){
       fprintf(stderr, "ERROR::FREETYTPE: Failed to load Glyph \n");  
       return -1;
       }

       glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    // load character glyph 
    if (FT_Load_Char(face, (int)'A', FT_LOAD_RENDER)){
    fprintf(stderr, "FreeType: Failed to load Glyph \n");
    return -1;
    }

    // generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RED,
    face->glyph->bitmap.width,
    face->glyph->bitmap.rows,
    0,
    GL_RED,
    GL_UNSIGNED_BYTE,
    face->glyph->bitmap.buffer
    );
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

    Character *character = (Character*)malloc(sizeof(Character));
    character->TextureID = texture;
    glm_vec2_copy((vec2){face->glyph->bitmap.width, face->glyph->bitmap.rows}, character->Size);
    glm_vec2_copy((vec2){face->glyph->bitmap_left, face->glyph->bitmap_top}, character->Bearing);
    character->Advance = face->glyph->advance.x;
    printf("TextureID: %d \n", character->TextureID);
    printf("Size: %f, %f \n", character->Size[0],character->Size[1]);
    printf("Bearing: %f, %f \n", character->Bearing[0],character->Bearing[1]);
    printf("Advance: %d \n", character->Advance);
    glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // enable byte-alignment restriction

    // configure VAO/VBO for texture quads (text)
    // -----------------------------------
    glGenVertexArrays(1, &VAOText);
    glGenBuffers(1, &VBOText);
    glBindVertexArray(VAOText);
    glBindBuffer(GL_ARRAY_BUFFER, VBOText);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    */

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)){
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(textShaderProgram);
        glUniform1i(glGetUniformLocation(textShaderProgram, "textColor"), 0);
        glUniform3f(textShaderProgram, 0.5f, 0.8f, 0.2f);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAOText);
        int x = 25;
        int y = 25;
        FrameBuffer *fb = framebuffer_create (FBDEV);
        for (int i = 0; i < (int)face->glyph->bitmap.rows; i++)
        {
            // row_offset is the distance from the top of the framebuffer
            //   of the text bounding box
            int row_offset = y + i + y_off;
            for (int j = 0; j < (int)face->glyph->bitmap.width; j++)
            {
                unsigned char p =
                    face->glyph->bitmap.buffer [i * face->glyph->bitmap.pitch + j];

                // Don't draw a zero value, unless you want to fill the bounding
                //   box with black.
                if (p)
                    framebuffer_set_pixel (fb, x + j + x_off, row_offset, p, p, p);
            }
        }
        // Move the x position, ready for the next character.
        x += advance;
        /*
           float x = 25.0;
           float y = 25.0;
           float scale = 1.0;
           float xpos = x + character->Bearing[0] * scale;
           float ypos = y - (character->Size[1] - character->Bearing[1]) * scale;

           float w = character->Size[0] * scale;
           float h = character->Size[1] * scale;
        // update VBO for each character
        float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 0.0f },            
        { xpos,     ypos,       0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, character->TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBOText);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (character->Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        */
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        }
        glfwTerminate();
        return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

FrameBuffer *framebuffer_create (const char *fbdev){
  FrameBuffer *self = malloc (sizeof (FrameBuffer));
  self->fbdev = strdup (fbdev);
  self->fd = -1;
  self->fb_data = NULL;
  self->fb_data_size = 0;
  return self;
}

void framebuffer_set_pixel (FrameBuffer *self, int x, int y, 
      BYTE r, BYTE g, BYTE b)
  {
  if (x > 0 && x < self->w && y > 0 && y < self->h)
    {
    int index32 = (y * self->w + x) * self->fb_bytes + y * self->slop;
    self->fb_data [index32++] = b;
    self->fb_data [index32++] = g;
    self->fb_data [index32++] = r;
    self->fb_data [index32] = 0;
    }
  }
