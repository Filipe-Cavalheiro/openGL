#include "shader_lib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cglm/include/cglm/cglm.h"
#include "../path-finding-in-C/aStar.h"

#define FOV M_PI/4
#define CAMERASPEED 2.5

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
float toRad(float deg);
unsigned int makeNsideShape(uint8_t sides, float diameter);
unsigned int makeSquare(float diameter);
unsigned int makeTriangle(float diameter);
void renderNsideShape(uint8_t sides);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//set up camera
vec3 cameraPos    = {0.0f, -5.0f,  0.0f};
vec3 cameraFront  = {0.0f, 1.0f,  0.0f};
vec3 cameraUp     = {-1.0f, 0.0f,  0.0f};

//time variables
float deltaTime = 0;

int main(){
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "grid", NULL, NULL);
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

    // build and compile shader program (grid)
    // ------------------------------------
    GLuint gridVS = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(&gridVS, GL_VERTEX_SHADER, "shaders/grid.vs");

    GLuint gridFS = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(&gridFS, GL_FRAGMENT_SHADER, "shaders/grid.fs");

    GLuint gridShader = glCreateProgram();
    link_shader(gridVS, gridFS, gridShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float grid_vertices[] = {
        // positions          
        1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,   
        -1.0f, -1.0f, 0.0f,   
        -1.0f,  1.0f, 0.0f, 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int GRID_VBO, GRID_VAO, GRID_EBO;
    glGenVertexArrays(1, &GRID_VAO);
    glGenBuffers(1, &GRID_VBO);
    glGenBuffers(1, &GRID_EBO);

    glBindVertexArray(GRID_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, GRID_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grid_vertices), grid_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GRID_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Clean Up, free all Vertex Array Objects
    glBindVertexArray(0);

    // build and compile shader program (BFS)
    // ------------------------------------
    GLuint BFS_VS = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(&BFS_VS, GL_VERTEX_SHADER, "shaders/BFS.vs");

    GLuint BFS_FS = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(&BFS_FS, GL_FRAGMENT_SHADER, "shaders/BFS.fs");

    GLuint BFS_shader = glCreateProgram();
    link_shader(BFS_VS, BFS_FS, BFS_shader);

    unsigned int NODE_VAO = makeNsideShape(6, 0.2f); 
    int squareState[6][6] = {
        {0,0,0,0,0,0},
        {0,0,0,0,0,0},
        {0,0,1,1,0,0},
        {0,0,1,0,0,0},
        {0,0,0,0,0,0},
        {0,0,0,0,0,0}
    };

    // build and compile shader program (LINK)
    // ------------------------------------
    GLuint LINK_VS = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(&LINK_VS, GL_VERTEX_SHADER, "shaders/link.vs");

    GLuint LINK_FS = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(&LINK_FS, GL_FRAGMENT_SHADER, "shaders/link.fs");

    GLuint LINK_shader = glCreateProgram();
    link_shader(LINK_VS, LINK_FS, LINK_shader);

    unsigned int LINK_VAO = makeNsideShape(4, 0.05f); 
    int linkCost[] = {
        255,255,255,255,255, //row        0
        255,1  ,255,255,255, //collum     5
        255,255,255,255,255, //diagonal   10
        1,255,255,1,255,    //row       15
        4,1,5,1,2,          //collum    20
        1,1,1,1,1,          //diagonal  25
        1,1,1,1,1,          //row       30
        1,1,1,1,1,          //collum    35
        1,1,1,255,1,        //diagonal  40
        1,1,1,1,1,          //row
        1,1,1,1,1,          //collum
        1,1,1,1,1,          //diagonal
        1,1,1,1,1,          //row
        1,1,1,1,1,          //collum
        1,1,1,1,1,          //diagonal
        1,1,1,1,1,          //row
        1,1,1,1,1,          //collum
        1,1,1,1,1           //diagonal
    };

    //coordinate-System variabels 
    mat4 matrix_model;
    mat4 matrix_view;
    mat4 matrix_projection;
    unsigned int modelLoc;
    unsigned int viewLoc;
    unsigned int projectionLoc;
    vec3 cameraTemp;

    //time variables
    float currentFrame;
    float lastFrame = 0;

    //path finding
    int startPos[2] = {0,0};
    int finishPos[2] = {3,3};
    aStar(startPos, finishPos, squareState, linkCost);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)){
        // per-frame time logic
        // --------------------
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //create transformations(grid)
        glUseProgram(gridShader);

        //enable varied opacity
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glm_mat4_identity(matrix_view);
        for(int i = 0; i < 3; ++i){cameraTemp[i] = cameraPos[i] + cameraFront[i];}
        glm_lookat(cameraPos, cameraTemp, cameraUp, matrix_view);
        viewLoc = glGetUniformLocation(gridShader, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *matrix_view);

        glm_mat4_identity(matrix_projection);
        glm_perspective(FOV, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, matrix_projection);
        projectionLoc = glGetUniformLocation(gridShader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, *matrix_projection);

        glBindVertexArray(GRID_VAO);
        glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, 0);
        glDisable(GL_BLEND);

        glUseProgram(LINK_shader);

        glm_mat4_identity(matrix_view);
        for(int i = 0; i < 3; ++i){cameraTemp[i] = cameraPos[i] + cameraFront[i];}
        glm_lookat(cameraPos, cameraTemp, cameraUp, matrix_view);
        viewLoc = glGetUniformLocation(BFS_shader, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *matrix_view);

        glm_mat4_identity(matrix_projection);
        glm_perspective(FOV, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, matrix_projection);
        projectionLoc = glGetUniformLocation(BFS_shader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, *matrix_projection);

        glBindVertexArray(LINK_VAO);
        int count = 0;
        unsigned int stateLoc;
        for(float i = -1.25; i < 1.75;){
            for(float j = -1; j < 1.5;){
                glm_mat4_identity(matrix_model);
                glm_translate(matrix_model, (vec3){i, 0, j});
                modelLoc = glGetUniformLocation(LINK_shader, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);

                stateLoc = glGetUniformLocation(LINK_shader, "state");
                glUniform1i(stateLoc, linkCost[count]);

                renderNsideShape(4);
                j += 0.5;
                ++count;
            }
            i += 0.25;
            if(i >= 1.5)
                break;
            for(float j = -1; j < 1.5;){
                glm_mat4_identity(matrix_model);
                glm_translate(matrix_model, (vec3){i, 0, j});
                modelLoc = glGetUniformLocation(LINK_shader, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);

                stateLoc = glGetUniformLocation(LINK_shader, "state");
                glUniform1i(stateLoc, linkCost[count]);

                renderNsideShape(4);
                j += 0.5;
                ++count;
            }
            for(float j = -1.25; j < 1.75;){
                glm_mat4_identity(matrix_model);
                glm_translate(matrix_model, (vec3){i, 0, j});
                modelLoc = glGetUniformLocation(LINK_shader, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);

                stateLoc = glGetUniformLocation(LINK_shader, "state");
                glUniform1i(stateLoc, linkCost[count]);

                renderNsideShape(4);
                j += 0.5;
                ++count;
            }
            i += 0.25;
        }

        glUseProgram(BFS_shader);

        glm_mat4_identity(matrix_view);
        for(int i = 0; i < 3; ++i){cameraTemp[i] = cameraPos[i] + cameraFront[i];}
        glm_lookat(cameraPos, cameraTemp, cameraUp, matrix_view);
        viewLoc = glGetUniformLocation(BFS_shader, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *matrix_view);

        glm_mat4_identity(matrix_projection);
        glm_perspective(FOV, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, matrix_projection);
        projectionLoc = glGetUniformLocation(BFS_shader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, *matrix_projection);

        glBindVertexArray(NODE_VAO);
        int row = 0;
        for(float i = -1.25; i < 1.75;){
            int collum = 0;
            for(float j = -1.25; j < 1.75;){
                glm_mat4_identity(matrix_model);
                glm_translate(matrix_model, (vec3){i, 0, j});
                modelLoc = glGetUniformLocation(BFS_shader, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);

                stateLoc = glGetUniformLocation(BFS_shader, "state");
                glUniform1i(stateLoc, squareState[row][collum]);

                renderNsideShape(6);
                j += 0.5;
                ++collum;
            }
            ++row;
            i += 0.5;
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &GRID_VAO);
    glDeleteBuffers(1, &GRID_VBO);
    glDeleteBuffers(1, &GRID_EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void renderNsideShape(uint8_t sides){
    if(sides == 3){
        glDrawArrays(GL_TRIANGLES, 0, 3);
        return;
    }
    glDrawElements(GL_TRIANGLE_FAN, sides+2, GL_UNSIGNED_INT, 0);
}

unsigned int makeNsideShape(uint8_t sides, float diameter){
    if(sides == 3)
        return makeTriangle(diameter);
    if (sides == 4){
        return makeSquare(diameter);
    }
    float* vertices = (float*)malloc(sizeof(float) * (sides+1) * 3);
    vertices[0] = 0;
    vertices[1] = 0;
    vertices[2] = 0;
    float angle = 2*M_PI/sides;
    for(int i = 3; i <= sides*3;){
        vertices[i] = sin(angle) * diameter;
        vertices[i + 1] = 0;
        vertices[i + 2] = cos(angle) * diameter;
        i += 3;
        angle -= 2*M_PI/sides;
    }
    int* indices = (int*)malloc(sizeof(int) * (sides+2));
    for(int i = 0; i <= sides;++i){
        indices[i] = i;
    }
    indices[sides+1] = 1;

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (sides+1) * 3, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * (sides+2), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0);
    free(vertices);
    free(indices);
    return VAO;
}

unsigned int makeTriangle(float diameter){
    float vertices[] = {
        -(float)diameter, 0.0f, -(float)diameter,  // left  
        (float)diameter, 0.0f, -(float)diameter,  // right 
        0.0f, 0.0f,   (float)diameter  // top   
    }; 

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
    return VAO; 
}

unsigned int makeSquare(float diameter){
    float square_vertices[] = {
        diameter,  0.0f, diameter,  // top right
        diameter, 0.0f, -diameter,  // bottom right
        -diameter, 0.0f, -diameter,  // bottom left
        -diameter,  0.0f, diameter   // top left 
    };
    unsigned int square_indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int SQUARE_VBO, SQUARE_VAO, SQUARE_EBO;
    glGenVertexArrays(1, &SQUARE_VAO);
    glGenBuffers(1, &SQUARE_VBO);
    glGenBuffers(1, &SQUARE_EBO);

    glBindVertexArray(SQUARE_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, SQUARE_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SQUARE_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // position attribute
    glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, sizeof(int), (void*)0);
    glEnableVertexAttribArray(1);

    //Clean Up, free all Vertex Array Objects
    glBindVertexArray(0);
    return SQUARE_VAO;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window){
    vec3 cameraTemp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, 1);
    }
    float frameCameraSpeed = deltaTime * CAMERASPEED;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        for(int i = 0; i < 3; ++i)
            cameraPos[i] += cameraUp[i] * frameCameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        for(int i = 0; i < 3; ++i)
            cameraPos[i] -= cameraUp[i] * frameCameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){ 
        for(int i = 0; i < 3; ++i)
            cameraTemp[i] = 0;
        glm_vec3_crossn(cameraFront, cameraUp, cameraTemp);
        glm_normalize(cameraTemp);
        for(int i = 0; i < 3; ++i)
            cameraPos[i] -= cameraTemp[i] * frameCameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        for(int i = 0; i < 3; ++i)
            cameraTemp[i] = 0;
        glm_vec3_crossn(cameraFront, cameraUp, cameraTemp);
        glm_normalize(cameraTemp);
        for(int i = 0; i < 3; ++i)
            cameraPos[i] += cameraTemp[i] * frameCameraSpeed;
    } 
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

float torad(float deg){
    return deg * (M_PI/180);
}
