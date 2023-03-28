#include <unistd.h>
#include <math.h>
#include "shader_lib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cglm/include/cglm/cglm.h"
#include "port.c"

#define PI 3.14159265359
#define FOV PI/4
#define CAMERASPEED 2.5
#define SENSITIVITY 0.1
#define BOXWIDTH 0.5
#define BOXLENGTH 0.5
#define BOXHEIGHT 0.5

typedef struct{
    vec3 position;
    vec3 size;
}Cube;

typedef struct{
    Cube* cube;
    void* next;
}No;

typedef struct{
    int size;
    No *head;
    No *tail;
}LinkedList;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
float toRad(float deg);
Cube* makeCube(vec3 position, vec3 size); 
No* makeNo(vec3 position, vec3 size);
LinkedList* makeLinkedList();
int addCube(LinkedList *linkedList, vec3 position, vec3 size);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//set up camera
vec3 cameraPos    = {0.0f, 0.0f,  3.0f};
vec3 cameraFront  = {0.0f, 0.0f, -1.0f};
vec3 cameraUp     = {0.0f, 1.0f,  0.0f};

//set up car
vec3 carPos = {0,0,0};
vec3 carSize = {1,1,1};
float carAngle = 0;

uint8_t firstMouse = 1;
uint8_t debug = 0;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  400.0f;
float lastY =  300.0f;

float deltaTime = 0;
float lastFrame = 0;

int main(){ 
    /*
       int fd = open (PORTNAME, O_RDWR | O_NOCTTY | O_SYNC );
       if (fd < 0){
       fprintf(stderr, "error %d opening %s: %s", errno, PORTNAME, strerror (errno));
       return -1;
       }

    //set_interface_attribs (fd);    // set speed to 9600 bps, 8N1 (no parity)

    char buf [128];
    uint8_t bytes_read;

    //soft restart the atmega328p
    char kickstart = '1';
    write(fd, &kickstart, sizeof(kickstart));

    memset(buf, '\0', sizeof(buf)); 
    bytes_read = read(fd, buf, sizeof buf);
    printf("N bytes: %d Output: %s", bytes_read, buf);
    */

    LinkedList *linkedList = makeLinkedList();
    if(linkedList == NULL){
        fprintf(stderr, "Failed to create linkedList\n");
        return -1;
    }
    addCube(linkedList, (vec3){1,1.5,1}, (vec3){1,2,1}); 
    addCube(linkedList, (vec3){1,-2,0}, (vec3){1,1,0.5});

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
    glfwSetCursorPosCallback(window, mouse_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    GLuint triangle_vertex = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(&triangle_vertex, GL_VERTEX_SHADER, "vertex.glsl");

    GLuint triangle_frag = glCreateShader(GL_VERTEX_SHADER);
    compile_shader(&triangle_frag, GL_FRAGMENT_SHADER, "fragment.glsl");

    GLuint shaderProgram = glCreateProgram();
    link_shader(triangle_vertex, triangle_frag, shaderProgram);

    glUseProgram(shaderProgram);
    // dealocate memory
    glUseProgram(0);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        //pos                               //texture
        -BOXLENGTH, -BOXHEIGHT, -BOXWIDTH,  0.0f, 0.0f,
        BOXLENGTH, -BOXHEIGHT, -BOXWIDTH,  1.0f, 0.0f,
        BOXLENGTH,  BOXHEIGHT, -BOXWIDTH,  1.0f, 1.0f,
        BOXLENGTH,  BOXHEIGHT, -BOXWIDTH,  1.0f, 1.0f,
        -BOXLENGTH,  BOXHEIGHT, -BOXWIDTH,  0.0f, 1.0f,
        -BOXLENGTH, -BOXHEIGHT, -BOXWIDTH,  0.0f, 0.0f,

        -BOXLENGTH, -BOXHEIGHT,  BOXWIDTH,  0.0f, 0.0f,
        BOXLENGTH, -BOXHEIGHT,  BOXWIDTH,  1.0f, 0.0f,
        BOXLENGTH,  BOXHEIGHT,  BOXWIDTH,  1.0f, 1.0f,
        BOXLENGTH,  BOXHEIGHT,  BOXWIDTH,  1.0f, 1.0f,
        -BOXLENGTH,  BOXHEIGHT,  BOXWIDTH,  0.0f, 1.0f,
        -BOXLENGTH, -BOXHEIGHT,  BOXWIDTH,  0.0f, 0.0f,

        -BOXLENGTH,  BOXHEIGHT,  BOXWIDTH,  1.0f, 0.0f,
        -BOXLENGTH,  BOXHEIGHT, -BOXWIDTH,  1.0f, 1.0f,
        -BOXLENGTH, -BOXHEIGHT, -BOXWIDTH,  0.0f, 1.0f,
        -BOXLENGTH, -BOXHEIGHT, -BOXWIDTH,  0.0f, 1.0f,
        -BOXLENGTH, -BOXHEIGHT,  BOXWIDTH,  0.0f, 0.0f,
        -BOXLENGTH,  BOXHEIGHT,  BOXWIDTH,  1.0f, 0.0f,

        BOXLENGTH,   BOXHEIGHT, BOXWIDTH,  1.0f, 0.0f,
        BOXLENGTH,   BOXHEIGHT,-BOXWIDTH,  1.0f, 1.0f,
        BOXLENGTH,  -BOXHEIGHT,-BOXWIDTH,  0.0f, 1.0f,
        BOXLENGTH,  -BOXHEIGHT,-BOXWIDTH,  0.0f, 1.0f,
        BOXLENGTH,  -BOXHEIGHT, BOXWIDTH,  0.0f, 0.0f,
        BOXLENGTH,   BOXHEIGHT, BOXWIDTH,  1.0f, 0.0f,

        -BOXLENGTH, -BOXHEIGHT, -BOXWIDTH,  0.0f, 1.0f,
        BOXLENGTH,  -BOXHEIGHT,-BOXWIDTH,  1.0f, 1.0f,
        BOXLENGTH,  -BOXHEIGHT, BOXWIDTH,  1.0f, 0.0f,
        BOXLENGTH,  -BOXHEIGHT, BOXWIDTH,  1.0f, 0.0f,
        -BOXLENGTH, -BOXHEIGHT,  BOXWIDTH,  0.0f, 0.0f,
        -BOXLENGTH, -BOXHEIGHT, -BOXWIDTH,  0.0f, 1.0f,

        -BOXLENGTH,  BOXHEIGHT, -BOXWIDTH,  0.0f, 1.0f,
        BOXLENGTH,   BOXHEIGHT,-BOXWIDTH,  1.0f, 1.0f,
        BOXLENGTH,   BOXHEIGHT, BOXWIDTH,  1.0f, 0.0f,
        BOXLENGTH,   BOXHEIGHT, BOXWIDTH,  1.0f, 0.0f,
        -BOXLENGTH,  BOXHEIGHT,  BOXWIDTH,  0.0f, 0.0f,
        -BOXLENGTH,  BOXHEIGHT, -BOXWIDTH,  0.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Clean Up, free all Vertex Array Objects
    glBindVertexArray(0);

    // load and create a texture 
    // -------------------------
    int width, height, nrChannels;
    unsigned char *data;

    unsigned int texture0;
    glGenTextures(1, &texture0);
    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, texture0);
    glBindTexture(GL_TEXTURE_2D, texture0); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
                                            // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("img/woodcontainer.jpg", &width, &height, &nrChannels, 0);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{fprintf(stderr, "Failed to load texture\n");}
    stbi_image_free(data);

    unsigned int texture1;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
                                            // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(1);  
    data = stbi_load("img/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{fprintf(stderr, "Failed to load texture\n");}
    stbi_image_free(data);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture0"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 1);

    // bind Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // render loop
    // -----------
    mat4 matrix_model;
    mat4 matrix_view;
    mat4 matrix_projection;
    unsigned int modelLoc;
    unsigned int viewLoc;
    unsigned int projectionLoc;
    float angle;
    float currentFrame;
    vec3 cameraTemp;
    No *no;
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    while (!glfwWindowShouldClose(window)){
        // per-frame time logic
        // --------------------
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //fps counter
        nbFrames++;
        if ( currentFrame - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
                                               // printf and reset timer
            printf("%d fps\n", nbFrames);
            nbFrames = 0;
            lastTime += 1.0;
        }


        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //activate shaders
        glUseProgram(shaderProgram);

        // camera/view transformation
        glm_mat4_identity(matrix_projection);
        glm_perspective(FOV, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, matrix_projection);
        projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, *matrix_projection);

        glm_mat4_identity(matrix_view);
        for(int i = 0; i < 3; ++i){
            cameraTemp[i] = cameraPos[i] + cameraFront[i];
        }
        glm_lookat(cameraPos, cameraTemp, cameraUp, matrix_view);
        viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *matrix_view);

        //render
        glBindVertexArray(VAO);

        //car
        glm_mat4_identity(matrix_model);
        glm_translate(matrix_model, carPos);
        glm_scale(matrix_model, carSize);/*
        mat4 matrix_tmp = {		
            cos(carAngle), 0, -sin(carAngle), 0,
            0, 1,           0, 0,
            sin(carAngle), 0,  cos(carAngle), 0,
            0, 0,           0, 1,
        };
        glm_mat4_mul(matrix_model,matrix_tmp,matrix_model);*/
        modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //walls
        no = linkedList->head;
        while(no != NULL){
            // calculate the model matrix for each object and pass it to shader before drawing
            glm_mat4_identity(matrix_model);
            glm_translate(matrix_model, no->cube->position);
            glm_scale(matrix_model, no->cube->size);
            modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            no = no->next;
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //dealocate memory
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window){/*
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
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS){
        if(debug){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            debug = 0;
            usleep(60000); //debouncing 60ms
        }   
        else{
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            debug = 1;
            usleep(60000); //debouncing 60ms
        }
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
            carPos[2] -= cos(carAngle) * frameCameraSpeed;
            carPos[0] -= sin(carAngle) * frameCameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
            carPos[2] += cos(carAngle) * frameCameraSpeed;
            carPos[0] += sin(carAngle) * frameCameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        carAngle += frameCameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        carAngle -= frameCameraSpeed;
    }
*/
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
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
    return deg * (PI/180);
}

Cube* makeCube(vec3 position, vec3 size){ 
    Cube *cube = (Cube*)malloc(sizeof(Cube));
    glm_vec3_copy(position ,cube->position);
    glm_vec3_copy(size ,cube->size);
    return cube;
}

No* makeNo(vec3 position, vec3 size){ 
    No *no = (No*)malloc(sizeof(No)); 
    no->cube = makeCube(position, size);
    no->next = NULL;
    return no;
}

LinkedList* makeLinkedList(){ 
    LinkedList *linkedList = (LinkedList*)malloc(sizeof(LinkedList));
    linkedList->head = NULL;
    linkedList->tail = NULL;
    return linkedList;
}

int addCube(LinkedList *linkedList, vec3 position, vec3 size){
    if(linkedList->head == NULL){
        linkedList->head = makeNo(position, size);
        if(linkedList->head == NULL) return 1;
        linkedList->tail = linkedList->head;
        return 0;
    }
    No *no = makeNo(position, size);
    if(no == NULL) return 1;
    linkedList->tail->next = no;
    linkedList->tail = no;
    return 0;
}
