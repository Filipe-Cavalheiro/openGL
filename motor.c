#include "motor.h"

typedef struct {
    vec3 size;
    vec3 position;
    vec3 axis;
    vec3 pivot;
    float angle;
    unsigned int VAO;
}*motor, _motor;

motor makeMotor(vec3 size, vec3 position, vec3 axis, vec3 pivot, float angle){
    motor elem = (motor)malloc(sizeof(_motor));
    elem->size[0] = size[0];
    elem->size[1] = size[1];
    elem->size[2] = size[2];
    elem->position[0] = position[0];
    elem->position[1] = position[1];
    elem->position[2] = position[2];
    elem->axis[0] = axis[0];
    elem->axis[1] = axis[1];
    elem->axis[2] = axis[2];
    elem->pivot[0] = pivot[0];
    elem->pivot[1] = pivot[1];
    elem->pivot[2] = pivot[2];
    elem->angle = angle;
    elem->VAO = makeCuboidVAO(size[0], size[1], size[2]);
    return elem;
}

void renderMotor(vec3 cameraPos, vec3 cameraFront, vec3 cameraUp, unsigned int shader, motor elem, linkedList motorList, int index){
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

    node motorNode = getIndex(motorList, index);
    for(int i = 2; i <= index; ++i){
        motor tmpMotor = getElem_node(motorNode);
        glm_rotate(matrix_model, tmpMotor->angle, tmpMotor->axis);
        motorNode = prevNode(motorNode);
    }

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

void rotateArm(linkedList list, int index, float angle){
    angle = -angle;
    node motorNode = getIndex(list, index);
    motor motor = getElem_node(motorNode);
    float rotationPoint[3] = {motor->pivot[0], motor->pivot[1], motor->pivot[2]};
    float axis[3] = {motor->axis[0], motor->axis[1], motor->axis[2]};
    float cosDiff;
    float sinDiff;
    cosDiff = cos(angle - motor->angle);
    sinDiff = sin(angle - motor->angle);
    motor->angle = angle;

    while (motorNode != NULL) {
        motor = getElem_node(motorNode);

        motor->position[0] -= rotationPoint[0];
        motor->position[1] -= rotationPoint[1];
        motor->position[2] -= rotationPoint[2];
        // Apply Rodrigues' rotation formula
        vec3 result = {
            motor->position[0] * (cosDiff + axis[0] * axis[0] * (1 - cosDiff)) + 
                motor->position[1] * (axis[0] * axis[1] * (1 - cosDiff) - axis[2] * sinDiff) + 
                motor->position[2] * (axis[0] * axis[2] * (1 - cosDiff) + axis[1] * sinDiff),

            motor->position[0] * (axis[1] * axis[0] * (1 - cosDiff) + axis[2] * sinDiff) + 
                motor->position[1] * (cosDiff + axis[1] * axis[1] * (1 - cosDiff)) + 
                motor->position[2] * (axis[1] * axis[2] * (1 - cosDiff) - axis[0] * sinDiff),

            motor->position[0] * (axis[2] * axis[0] * (1 - cosDiff) - axis[1] * sinDiff) + 
                motor->position[1] * (axis[2] * axis[1] * (1 - cosDiff) + axis[0] * sinDiff) + 
                motor->position[2] * (cosDiff + axis[2] * axis[2] * (1 - cosDiff))
        };
        motor->position[0] = result[0];
        motor->position[1] = result[1];
        motor->position[2] = result[2];
        motor->position[0] += rotationPoint[0];
        motor->position[1] += rotationPoint[1];
        motor->position[2] += rotationPoint[2];
        //printf("New Position: %f, %f, %f\n", motor->position[0], motor->position[1], motor->position[2]);

        motor->pivot[0] -= rotationPoint[0];
        motor->pivot[1] -= rotationPoint[1];
        motor->pivot[2] -= rotationPoint[2];
        result[0] = motor->pivot[0] * (cosDiff + axis[0] * axis[0] * (1 - cosDiff)) + 
            motor->pivot[1] * (axis[0] * axis[1] * (1 - cosDiff) - axis[2] * sinDiff) + 
            motor->pivot[2] * (axis[0] * axis[2] * (1 - cosDiff) + axis[1] * sinDiff);

        result[1] = motor->pivot[0] *(axis[1] * axis[0] * (1 - cosDiff) + axis[2] * sinDiff) + 
            motor->pivot[1] * (cosDiff + axis[1] * axis[1] * (1 - cosDiff)) + 
            motor->pivot[2] * (axis[1] * axis[2] * (1 - cosDiff) - axis[0] * sinDiff);

        result[2] = motor->pivot[0] *(axis[2] * axis[0] * (1 - cosDiff) - axis[1] * sinDiff) + 
            motor->pivot[1] * (axis[2] * axis[1] * (1 - cosDiff) + axis[0] * sinDiff) + 
            motor->pivot[2] * (cosDiff + axis[2] * axis[2] * (1 - cosDiff));
        motor->pivot[0] = result[0];
        motor->pivot[1] = result[1];
        motor->pivot[2] = result[2];
        motor->pivot[0] += rotationPoint[0];
        motor->pivot[1] += rotationPoint[1];
        motor->pivot[2] += rotationPoint[2];
        //printf("New Pivot: %f, %f, %f\n", motor->pivot[0], motor->pivot[1], motor->pivot[2]);


        motorNode = nextNode(motorNode);
    }
}
