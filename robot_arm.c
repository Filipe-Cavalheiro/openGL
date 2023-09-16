#include "shader_lib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cglm/include/cglm/cglm.h"
#include "shapes.h"
#include "../Data-structures-in-C/linkedList.h"

#define FOV M_PI/4
#define CAMERASPEED 2.5
#define SENSITIVITY 0.1
#define ARROWKEYS '-'

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
float toRad(float deg);
void set_currently_pressed_key(uint8_t key);
void clear_currently_pressed_key(uint8_t key);
uint8_t get_currently_pressed_key(uint8_t key);

// settings
#define SCR_WIDTH 800
#define SCR_HEIGHT 600

//set up camera
vec3 cameraPos    = {0.0f, 1.0f,  0.0f};
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

float generalAngle = 0;
uint8_t currently_pressed_keys[30] = {0};

typedef struct {
    vec3 size;
    vec3 position;
    vec3 pivot;
    float angle;
    unsigned int VAO;
    mat4 matrix;
}*robotPart, _robotPart;

robotPart makeRobotPart(vec3 size, vec3 position, vec3 pivot, float angle){
    robotPart elem = (robotPart)malloc(sizeof(_robotPart));
    elem->size[0] = size[0];
    elem->size[1] = size[1];
    elem->size[2] = size[2];
    elem->position[0] = position[0];
    elem->position[1] = position[1];
    elem->position[2] = position[2];
    elem->pivot[0] = pivot[0];
    elem->pivot[1] = pivot[1];
    elem->pivot[2] = pivot[2];
    elem->angle = angle;
    elem->VAO = makeCuboid(size[0], size[1], size[2]);
    return elem;
}

void rotateArm(linkedList list, int index, float angle){
    node motorNode = getHead(list);
    for(int i = 0; i < index; ++i){
        if(motorNode == NULL)
            return;
        motorNode = nextNode(motorNode);
    }
    float result[2] = {0,0};
    robotPart motor;
    while(motorNode != NULL){                  
        motor = getElem_node(motorNode);
        float diff = angle - motor->angle; 
        if(motor->angle != angle){
            result[0] = motor->position[0] * cos(diff) - motor->position[2] * sin(diff);
            result[1] = motor->position[0] * sin(diff) + motor->position[2] * cos(diff);
            motor->position[0] = result[0];
            motor->position[2] = result[1];
        }
        motor->angle = angle;
        motorNode = nextNode(motorNode);
    }
}

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    // build and compile shader program (cube)
    // ------------------------------------
    GLuint cubeVS = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(&cubeVS, GL_VERTEX_SHADER, "shaders/cube.vs");

    GLuint cubeFS = glCreateShader(GL_FRAGMENT_SHADER);
    compile_shader(&cubeFS, GL_FRAGMENT_SHADER, "shaders/cube.fs");

    GLuint cubeShader = glCreateProgram();
    link_shader(cubeVS, cubeFS, cubeShader);

    linkedList list = makeLinkedList();
    robotPart motor;
    motor = makeRobotPart((vec3){0.8f, 0.2f, 0.8f}, (vec3){0.0f, 0.2f, 0.0f}, (vec3){0, 1, 0}, M_PI/2);
    append(list, motor); 
    motor = makeRobotPart((vec3){0.2f, 1.0f, 0.2f}, (vec3){0.0f, 1.4f, 0.0f}, (vec3){0, 1, 0}, 0);
    append(list, motor); 
    motor = makeRobotPart((vec3){1.0f, 0.2f, 0.2f}, (vec3){0.7f, 2.4f, 0.0f}, (vec3){0, 1, 0}, 0);
    append(list, motor);
    printf("position: %f, %f\n", motor->position[0], motor->position[2]);
    motor = makeRobotPart((vec3){0.2f, 0.2f, 1.0f}, (vec3){1.9f, 2.4f, 0.8f}, (vec3){0, 1, 0}, 0);
    append(list, motor); 
    motor = makeRobotPart((vec3){0.2f, 1.0f, 0.2f}, (vec3){2.3f, 3.2f, 1.6f}, (vec3){0, 1, 0}, 0);
    append(list, motor); 
    motor = makeRobotPart((vec3){0.4f, 0.2f, 0.2f}, (vec3){2.3f, 4.4f, 1.6f}, (vec3){0, 1, 0}, 0);
    append(list, motor);
    
    //Coordinate-System variabels 
    mat4 matrix_view;
    mat4 matrix_projection;
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

        //enable varied opacity
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        //create transformations(grid)
        glUseProgram(gridShader);

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
        
        rotateArm(list, 2, generalAngle);

        node motorNode = getHead(list);
        while(motorNode != NULL){
            motor = getElem_node(motorNode);
            renderCuboid(cameraPos, cameraFront, cameraUp, cubeShader, motor->VAO, motor->position, motor->pivot, -motor->angle);
            motorNode = nextNode(motorNode);
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
    if(get_currently_pressed_key('q') && glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE){
        clear_currently_pressed_key('q');
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        if(!get_currently_pressed_key('q')){    
            glfwSetWindowShouldClose(window, 1);
            set_currently_pressed_key('q');
        }
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
    /*
    if(get_currently_pressed_key('l') && glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE){
        clear_currently_pressed_key('l');
    }*/
    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
        //if(!get_currently_pressed_key('l')){   
            generalAngle += 0.05;
            //set_currently_pressed_key('l');
        //}
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

void set_currently_pressed_key(uint8_t key){
    if(key == ARROWKEYS){ 
        currently_pressed_keys[27] = 1;
        return;
    }
    currently_pressed_keys[key - 'a'] = 1;
}

void clear_currently_pressed_key(uint8_t key){
    if(key == ARROWKEYS){ 
        currently_pressed_keys[27] = 0;
        return;
    }
    currently_pressed_keys[key - 'a'] = 0;
}

uint8_t get_currently_pressed_key(uint8_t key){
    if(key == ARROWKEYS){ 
        return currently_pressed_keys[27];
    }
    return currently_pressed_keys[key - 'a'];
}
