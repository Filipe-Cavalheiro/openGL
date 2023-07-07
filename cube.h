#ifndef CUBE_H
#define CUBE_H

#include "cube.c"

No* makeNo(void* pointer2Struct);
LinkedList* makeLinkedList();
Cube* makeCube(vec3 position, vec3 size); 
int addCube(LinkedList *linkedList, vec3 position, vec3 size);

#endif /* __CUBE_H__ */
