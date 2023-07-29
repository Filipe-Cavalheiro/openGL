#include "shader_lib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cglm/include/cglm/cglm.h"

#define FOV M_PI/4
#define CAMERASPEED 2.5
#define SENSITIVITY 0.1

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
float toRad(float deg);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//set up camera
vec3 cameraPos    = {0.0f, 5.0f,  0.0f};
vec3 cameraFront  = {0.0f, -1.0f,  0.0f};
vec3 cameraUp     = {1.0f, 0.0f,  0.0f};

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

    // build and compile shader program (square)
    // ------------------------------------
    GLuint A_starVS = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(&A_starVS, GL_VERTEX_SHADER, "shaders/A_star.vs");

    GLuint A_starFS = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(&A_starFS, GL_FRAGMENT_SHADER, "shaders/A_star.fs");

    GLuint A_starShader = glCreateProgram();
    link_shader(A_starVS, A_starFS, A_starShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float square_vertices[] = {
        0.1f,  0.0f, 0.1f,  // top right
        0.1f, 0.0f, -0.1f,  // bottom right
        -0.1f, 0.0f, -0.1f,  // bottom left
        -0.1f,  0.0f, 0.1f   // top left 
    };
    unsigned int square_indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    int square_state[] = {
        1,1,1,1,1,
        1,0,0,0,1,
        1,0,0,1,1,
        1,0,0,0,1,
        1,1,1,1,1
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
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDisable(GL_BLEND);

        //create transformations(square)
        glUseProgram(A_starShader);

        glm_mat4_identity(matrix_view);
        for(int i = 0; i < 3; ++i){cameraTemp[i] = cameraPos[i] + cameraFront[i];}
        glm_lookat(cameraPos, cameraTemp, cameraUp, matrix_view);
        viewLoc = glGetUniformLocation(A_starShader, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *matrix_view);

        glm_mat4_identity(matrix_projection);
        glm_perspective(FOV, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, matrix_projection);
        projectionLoc = glGetUniformLocation(A_starShader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, *matrix_projection);

        glBindVertexArray(SQUARE_VAO);
        int count = 0;
        unsigned int stateLoc;
        for(float i = -1; i < 1.5;){
                for(float j = -1; j < 1.5;){
                    glm_mat4_identity(matrix_model);
                    glm_translate(matrix_model, (vec3){i, 0, j});
                    modelLoc = glGetUniformLocation(A_starShader, "model");
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);
                    
                    stateLoc = glGetUniformLocation(A_starShader, "state");
                    glUniform1i(stateLoc, square_state[count]);
                    
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    j += 0.5;
                    ++count;
                }
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
