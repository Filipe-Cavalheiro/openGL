#include "shader_lib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cglm/include/cglm/cglm.h"

#define FOV M_PI/4
#define CAMERASPEED 2.5
#define SENSITIVITY 0.1

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
float toRad(float deg);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//set up camera
vec3 cameraPos    = {0.0f, 0.0f,  3.0f};
vec3 cameraFront  = {0.0f, 0.0f, -1.0f};
vec3 cameraUp     = {0.0f, 1.0f,  0.0f};

//time variables
float deltaTime = 0;

//mouse variables
uint8_t firstMouse = 1;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  400.0f;
float lastY =  300.0f;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Template of opengl", NULL, NULL);
    if (window == NULL){
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    // build and compile shader program (elements)
    // ------------------------------------
    GLuint triangle_vertex = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(&triangle_vertex, GL_VERTEX_SHADER, "shaders/template.vs");

    GLuint triangle_frag = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(&triangle_frag, GL_FRAGMENT_SHADER, "shaders/template.fs");

    GLuint elementsShader = glCreateProgram();
    link_shader(triangle_vertex, triangle_frag, elementsShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,   
        -0.5f, -0.5f, 0.0f,   
        -0.5f,  0.5f, 0.0f, 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
 
    //Coordinate-System variabels 
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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
 
        //activate shaders
        glUseProgram(elementsShader);

        //create transformations
        glm_mat4_identity(matrix_model);
        modelLoc = glGetUniformLocation(elementsShader, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);

        // calculate the model matrix for each object and pass it to shader before drawing
        glm_mat4_identity(matrix_view);
        for(int i = 0; i < 3; ++i){cameraTemp[i] = cameraPos[i] + cameraFront[i];}
        glm_lookat(cameraPos, cameraTemp, cameraUp, matrix_view);
        viewLoc = glGetUniformLocation(elementsShader, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *matrix_view);

        glm_mat4_identity(matrix_projection);
        glm_perspective(FOV, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, matrix_projection);
        projectionLoc = glGetUniformLocation(elementsShader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, *matrix_projection);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
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
            cameraPos[i] += cameraFront[i] * frameCameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        for(int i = 0; i < 3; ++i)
            cameraPos[i] -= cameraFront[i] * frameCameraSpeed;
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        for(int i = 0; i < 3; ++i)
            cameraPos[i] += cameraUp[i] * frameCameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        for(int i = 0; i < 3; ++i)
            cameraPos[i] -= cameraUp[i] * frameCameraSpeed;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = 0;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= SENSITIVITY;
    yoffset *= SENSITIVITY;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    vec3 front;
    front[0] = cos(toRad(yaw)) * cos(toRad(pitch));
    front[1] = sin(toRad(pitch));
    front[2] = sin(toRad(yaw)) * cos(toRad(pitch));
    glm_normalize(front);
    for(int i = 0; i < 3; ++i)
        cameraFront[i] = front[i];
    return;
}

float toRad(float deg){
    return deg * (M_PI/180);
}
