#include <string.h>
#include "cglm/include/cglm/cglm.h"

void rotateZ(vec3 position, float angle){
    printf("Starting pos: %f, %f, %f\n", position[0], position[1], position[2]);
    float matrix[3][3];
    memset(matrix, 0, sizeof(matrix));
    matrix[0][0] = position[0];
    matrix[1][1] = position[2];
    matrix[2][2] = position[1];

    float cosA = cosf(angle);
    float sinA = sinf(angle);

    float rotationMatrix[3][3] = {
        {cosA, -sinA, 0},
        {sinA, cosA, 0},
        {0, 0, 1}
    };

    float resultMatrix[3][3];
    memset(resultMatrix, 0, sizeof(resultMatrix));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                resultMatrix[i][j] += matrix[i][k] * rotationMatrix[k][j];
            }
        }
    }

    float result[2];
    result[0] = resultMatrix[0][0] * position[0] + resultMatrix[0][2] * position[1] + resultMatrix[0][2] * position[1];
    result[1] = resultMatrix[1][0] * position[0] + resultMatrix[1][2] * position[1] + resultMatrix[1][2] * position[1];
    position[0] = result[0];
    position[2] = result[1];
    printf("New pos: %f, %f, %f\n", position[0], position[1], position[2]);
}


int main(){
    vec3 position = {0.7f,0.2f,0.0f};
    rotateZ(position, M_PI/2);
    return 0;
}
