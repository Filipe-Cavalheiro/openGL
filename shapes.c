#include "shapes.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define FOV M_PI/4

void renderNsideShape(uint8_t sides){
    if(sides == 3){
        glDrawArrays(GL_TRIANGLES, 0, 3);
        return;
    }
    glDrawElements(GL_TRIANGLE_FAN, sides+2, GL_UNSIGNED_INT, 0);
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

unsigned int makeRectangle(float length, float width){
    length /= 2;
    width /= 2;
    float square_vertices[] = {
        width,  0.0f, length,  // top ritht
        width, 0.0f, -length,  // bottom right
        -width, 0.0f, -length,  // bottom left
        -width,  0.0f, length   // top left 
    };
    unsigned int square_indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int SQUARE_VBO, RECTANGLE_VAO, SQUARE_EBO;
    glGenVertexArrays(1, &RECTANGLE_VAO);
    glGenBuffers(1, &SQUARE_VBO);
    glGenBuffers(1, &SQUARE_EBO);

    glBindVertexArray(RECTANGLE_VAO);

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
    return RECTANGLE_VAO;
}

unsigned int makeTriangle(float diameter){
    float vertices[] = {
        -diameter, 0.0f, -diameter,  // left  
        diameter, 0.0f, -diameter,  // right 
        0.0f, 0.0f,   diameter  // top   
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

unsigned int makeSphere(int numSlices,int numStacks,float radius){
    // Create a simple wireframe sphere
    float sphere_vertices[(numSlices + 1) * (numStacks + 1) * 3];

    int index = 0;
    for (int i = 0; i <= numStacks; ++i){
        float theta = i * M_PI / numStacks;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int j = 0; j <= numSlices; ++j){
            float phi = j * 2 * M_PI / numSlices;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            sphere_vertices[index++] = radius * x;
            sphere_vertices[index++] = radius * y;
            sphere_vertices[index++] = radius * z;
        }
    }

    GLuint SPHERE_VBO, SPHERE_VAO;
    glGenVertexArrays(1, &SPHERE_VAO);
    glGenBuffers(1, &SPHERE_VBO);

    glBindVertexArray(SPHERE_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, SPHERE_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Clean Up, free all Vertex Array Objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return SPHERE_VAO;
}

void renderSphere(vec3 cameraPos, vec3 cameraFront, vec3 cameraUp, int numSlices, int numStacks, unsigned int shader, unsigned int VAO){
    mat4 matrix_model;
    mat4 matrix_view;
    mat4 matrix_projection;
    unsigned int modelLoc;
    unsigned int viewLoc;
    unsigned int colorLoc;
    unsigned int projectionLoc;
    vec3 cameraTemp;

    // Create transformation (sphere)
    glUseProgram(shader);

    glm_mat4_identity(matrix_model);
    glm_translate(matrix_model, (vec3){0.0f, 0.5f, 0.0f});
    modelLoc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);

    // calculate the model matrix for each object and pass it to shader before drawing
    glm_mat4_identity(matrix_view);
    for(int i = 0; i < 3; ++i){cameraTemp[i] = cameraPos[i] + cameraFront[i];}
    glm_lookat(cameraPos, cameraTemp, cameraUp, matrix_view);
    viewLoc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *matrix_view);

    glm_mat4_identity(matrix_projection);
    glm_perspective(FOV, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, matrix_projection);
    projectionLoc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, *matrix_projection);

    colorLoc = glGetUniformLocation(shader, "color");
    glUniform1f(colorLoc, 0.3f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, (numSlices + 1) * (numStacks + 1));
}

typedef struct {
    vec3 size;
    vec3 position;
    vec3 angles;
    float angle;
    unsigned int VAO;
}_cuboid, *cuboid;

unsigned int makeCuboidVAO(float width, float height, float lenght){
    float cube_vertices[] = {
        -width, -height, -lenght,
        width, -height, -lenght,
        width,  height, -lenght,  
        width,  height, -lenght,  
        -width,  height, -lenght,  
        -width, -height, -lenght,  

        -width, -height,  lenght,  
        width, -height,  lenght,  
        width,  height,  lenght,  
        width,  height,  lenght,  
        -width,  height,  lenght,  
        -width, -height,  lenght,  

        -width,  height,  lenght,  
        -width,  height, -lenght,  
        -width, -height, -lenght,  
        -width, -height, -lenght,  
        -width, -height,  lenght,  
        -width,  height,  lenght,  

        width,  height,  lenght,  
        width,  height, -lenght,  
        width, -height, -lenght,  
        width, -height, -lenght,  
        width, -height,  lenght,  
        width,  height,  lenght,  

        -width, -height, -lenght,  
        width, -height, -lenght,  
        width, -height,  lenght,  
        width, -height,  lenght,  
        -width, -height,  lenght,  
        -width, -height, -lenght,  

        -width,  height, -lenght,  
        width,  height, -lenght,  
        width,  height,  lenght,  
        width,  height,  lenght,  
        -width,  height,  lenght,  
        -width,  height, -lenght,  
    };
    unsigned int CUBE_VBO, CUBE_VAO;
    glGenVertexArrays(1, &CUBE_VAO);
    glGenBuffers(1, &CUBE_VBO);

    glBindVertexArray(CUBE_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, CUBE_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Clean Up, free all Vertex Array Objects
    glBindVertexArray(0);
    glDeleteBuffers(1, &CUBE_VBO);
    return CUBE_VAO;
}

cuboid makeCuboid(vec3 size, vec3 position, vec3 angles){
    cuboid elem = (cuboid)malloc(sizeof(_cuboid));
    elem->size[0] = size[0];
    elem->size[1] = size[1];
    elem->size[2] = size[2];
    elem->position[0] = position[0];
    elem->position[1] = position[1];
    elem->position[2] = position[2];
    elem->angles[0] = angles[0];
    elem->angles[1] = angles[1];
    elem->angles[2] = angles[2];
    elem->VAO = makeCuboidVAO(size[0], size[1], size[2]);
    return elem;
}

void renderCuboid(vec3 cameraPos, vec3 cameraFront, vec3 cameraUp, unsigned int shader, cuboid elem){
    mat4 matrix_model;
    mat4 matrix_view;
    mat4 matrix_projection;
    unsigned int modelLoc;
    unsigned int viewLoc;
    unsigned int projectionLoc;
    vec3 cameraTemp;
    glUseProgram(shader);

    glm_mat4_identity(matrix_model);
    glm_translate(matrix_model, elem->position);
    glm_rotate(matrix_model, elem->angles[0], (vec3){1, 0, 0});
    glm_rotate(matrix_model, elem->angles[1], (vec3){0, 1, 0});
    glm_rotate(matrix_model, elem->angles[2], (vec3){0, 0, 1});
    modelLoc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *matrix_model);

    // calculate the model matrix for each object and pass it to shader before drawing
    glm_mat4_identity(matrix_view);
    for(int i = 0; i < 3; ++i){cameraTemp[i] = cameraPos[i] + cameraFront[i];}
    glm_lookat(cameraPos, cameraTemp, cameraUp, matrix_view);
    viewLoc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *matrix_view);

    glm_mat4_identity(matrix_projection);
    glm_perspective(FOV, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f, matrix_projection);
    projectionLoc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, *matrix_projection);

    glBindVertexArray(elem->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
