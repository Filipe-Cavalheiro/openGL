#include <string.h>
#include "cglm/include/cglm/cglm.h"

void rotateZ(vec3 position, float angle, float resultMatrix[3][3]){
    float matrix[3][3];
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

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                resultMatrix[i][j] += matrix[i][k] * rotationMatrix[k][j];
            }
        }
    }

    float result[3];
    result[0] = resultMatrix[0][0] * position[0] + resultMatrix[0][1] * position[2] + resultMatrix[0][2] * position[1];
    result[1] = resultMatrix[1][0] * position[0] + resultMatrix[1][1] * position[2] + resultMatrix[1][2] * position[1];
    result[2] = resultMatrix[2][0] * position[0] + resultMatrix[2][1] * position[2] + resultMatrix[2][2] * position[1];
    position[0] = result[0];
    position[1] = result[1];
    position[2] = result[2];
}

void translate(vec3 position, float dx, float dy, float dz) {
    position[0] += dx;
    position[1] += dy;
    position[2] += dz;
}

int main(){
    vec3 position = {1,0,1};
    float matrix[3][3];
    memset(matrix, 0, sizeof(matrix));
    translate(position, -1, 0, -1);
    rotateZ(position, M_PI/4, matrix);
    translate(position, 1, 0, 1);
    printf("position: %f, %f, %f\n", position[0], position[1], position[2]);
    return 0;
}
