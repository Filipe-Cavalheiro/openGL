#ifndef MOTOR_H
#define MOTOR_H

#include "motor.c"

motor makeMotor(vec3 size, vec3 position, vec3 axis, vec3 pivot, float angle);
void renderMotor(vec3 cameraPos, vec3 cameraFront, vec3 cameraUp, unsigned int shader, motor elem, linkedList motorList, int index);
void rotateArm(linkedList list, int index, float angle);

#endif /* __MOTOR_H__ */
