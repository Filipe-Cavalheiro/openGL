#ifndef CUBE_H
#define CUBE_H

#include "data_structures/linkedList.h"
#include "cube.c"

Cube* makeCube(vec3 position, vec3 size); 
int addCube(LinkedList *linkedList, vec3 position, vec3 size);

#endif /* __CUBE_H__ */
