#include "shader_lib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cglm/include/cglm/cglm.h"
#include "../Data-structures-in-C/linkedList.h"
#include "shapes.h"

#define FOV M_PI/4
#define CAMERASPEED 2.5

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
float toRad(float deg);
void renderChain(unsigned int RECTANGLE_VAO, unsigned int PIVOT_VAO, unsigned int shader, vec3 camerePos, vec3 cameraFront, linkedList list);

typedef struct {
    float pos[2];
    float angle;
}_chainNode, *chainNode;

chainNode makeChainNode(float pos[2], float angle){
    chainNode elem = (chainNode)malloc(sizeof(_chainNode));
    elem->pos[0] = pos[0];
    elem->pos[1] = pos[1];
    elem->angle = angle;
    return elem;
}

chainNode joinChain(linkedList list, float angle){
    if(list == NULL)
        return NULL;
    node parentNode = getHead(list);
    if(parentNode == NULL)
        return NULL;
    while(nextNode(parentNode) != NULL)
        parentNode = nextNode(parentNode);
    chainNode elemChain = (chainNode)getElem_node(parentNode);
    float newNodePos[2];
    newNodePos[0] = (elemChain->pos[0] + 0.7*cos(elemChain->angle) + 0.7*cos(elemChain->angle + angle));
    newNodePos[1] = (elemChain->pos[1] + 0.7*sin(elemChain->angle) + 0.7*sin(elemChain->angle + angle));
    chainNode newNode = makeChainNode(newNodePos, elemChain->angle + angle);
    append(list, newNode);
    return newNode;
}

void print_chainNode(void* data){
    chainNode node = (chainNode)data;
    printf("pos:%f, %f\n", node->pos[0], node->pos[1]);
    printf("angle:%f\n", node->angle);
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//set up camera
vec3 cameraPos    = {0.0f, -5.0f,  0.0f};
vec3 cameraFront  = {0.0f, 1.0f,  0.0f};
vec3 cameraUp     = {0.0f, 0.0f,  1.0f};

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
        exit(EXIT_FAILURE);
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
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

    // build and compile shader program (UNIFORM)
    // ------------------------------------
    GLuint UNIFORM_VS = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(&UNIFORM_VS, GL_VERTEX_SHADER, "shaders/uniform.vs");

    GLuint UNIFORM_FS = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(&UNIFORM_FS, GL_FRAGMENT_SHADER, "shaders/uniform.fs");

    GLuint UNIFORM_shader = glCreateProgram();
    link_shader(UNIFORM_VS, UNIFORM_FS, UNIFORM_shader);

    unsigned int PIVOT_VAO = makeNsideShape(8, 0.2f); 
    unsigned int RECTANGLE_VAO = makeRectangle(0.6f, 2.0f); 

    //Render Variables 
    mat4 matrix_view;
    mat4 matrix_projection;
    unsigned int viewLoc;
    unsigned int projectionLoc;
    vec3 cameraTemp;

    //time variables
    float currentFrame;
    float lastFrame = 0;

    //make chains
    linkedList list = makeLinkedList();
    chainNode chainStart = makeChainNode((float[2]){0.0,0.0}, 0);
    append(list, chainStart);
    joinChain(list, -M_PI/5);
    joinChain(list, M_PI/2);
    print_ll(list, print_chainNode);
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


        renderChain(RECTANGLE_VAO, PIVOT_VAO, UNIFORM_shader, cameraPos, cameraFront, list);

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

void renderChain(unsigned int RECTANGLE_VAO, unsigned int PIVOT_VAO, unsigned int shader, vec3 camerePos, vec3 cameraFront, linkedList list){
    mat4 matrix_model;
    mat4 matrix_view;
    mat4 matrix_projection;
    unsigned int modelLoc;
    unsigned int viewLoc;
    unsigned int projectionLoc;
    unsigned int colorLoc;
    vec3 cameraTemp;

    //create transformations(uniform)
    glUseProgram(shader);

    node node = getHead(list);
    chainNode elem;
    while(node != NULL){
        elem = (chainNode)getElem_node(node);
        /***RECTANGLE***/
        glm_mat4_identity(matrix_view);
        for(int i = 0; i < 3; ++i){cameraTemp[i] = cameraPos[i] + cameraFront[i];}
        glm_lookat(cameraPos, cameraTemp, cameraUp, matrix_view);
        viewLoc = glGetUniformLocation(shader, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *matrix_view);

        glm_mat4_identity(matrix_projection);
        glm_perspective(FOV, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, matrix_projection);
        projectionLoc = glGetUniformLocation(shader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, *matrix_projection);
        glBindVertexArray(RECTANGLE_VAO);

        glm_mat4_identity(matrix_model);
        glm_translate(matrix_model, (vec3){(elem->pos)[0], 0, elem->pos[1]});
        glm_rotate_at(matrix_model, (vec3){0, 0, 0}, elem->angle, (vec3){0, -1, 0});
        modelLoc = glGetUniformLocation(shader, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);
        colorLoc = glGetUniformLocation(shader, "color");
        glUniform1f(colorLoc, 0.3f);
        renderNsideShape(4);

        /***PIVOT***/
        for(float i = -0.7; i < 0.75; i += 1.4){
            glm_mat4_identity(matrix_view);
            for(int i = 0; i < 3; ++i){cameraTemp[i] = cameraPos[i] + cameraFront[i];}
            glm_lookat(cameraPos, cameraTemp, cameraUp, matrix_view);
            viewLoc = glGetUniformLocation(shader, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *matrix_view);

            glm_mat4_identity(matrix_projection);
            glm_perspective(FOV, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, matrix_projection);
            projectionLoc = glGetUniformLocation(shader, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, *matrix_projection);
            glBindVertexArray(PIVOT_VAO);

            glm_mat4_identity(matrix_model);
            glm_translate(matrix_model, (vec3){elem->pos[0] + i, 0, elem->pos[1]});
            glm_rotate_at(matrix_model, (vec3){0.0 - i, 0, 0}, elem->angle, (vec3){0, -1, 0});
            modelLoc = glGetUniformLocation(shader, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);
            colorLoc = glGetUniformLocation(shader, "color");
            glUniform1f(colorLoc, 0.1f);
            renderNsideShape(8);
        }
        node = nextNode(node);
    }
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
