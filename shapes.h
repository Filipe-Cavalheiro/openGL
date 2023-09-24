#ifndef SHAPES_H
#define SHAPES_H

#include "shapes.c"

void renderNsideShape(uint8_t sides);
unsigned int makeSquare(float diameter);
unsigned int makeRectangle(float width, float length);
unsigned int makeTriangle(float diameter);
unsigned int makeNsideShape(uint8_t sides, float diameter);
unsigned int makeSphere(int numSplices,int numStacks,float radius);
void renderSphere(vec3 cameraPos, vec3 cameraFront, vec3 cameraUp, int numSlices, int numStacks, unsigned int shader, unsigned int VAO);
unsigned int makeCuboidVAO(float width, float height, float lenght);
cuboid makeCuboid(vec3 size, vec3 position, vec3 angles);
void renderCuboid(vec3 cameraPos, vec3 cameraFront, vec3 cameraUp, unsigned int shader, cuboid elem);

#endif /* SHAPES_H */
