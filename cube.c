#include "cube.h"

typedef struct{
    vec3 position;
    vec3 size;
}Cube;

Cube* makeCube(vec3 position, vec3 size){ 
    Cube *cube = (Cube*)malloc(sizeof(Cube));
    glm_vec3_copy(position ,cube->position);
    glm_vec3_copy(size ,cube->size);
    return cube;
}

int addCube(LinkedList *linkedList, vec3 position, vec3 size){
    if(linkedList->head == NULL){
        linkedList->head = makeNo(makeCube(position, size));
        if(linkedList->head == NULL) return 1;
        linkedList->tail = linkedList->head;
        return 0;
    }
    No *no = makeNo(makeCube(position, size));
    if(no == NULL) return 1;
    linkedList->tail->next = no;
    linkedList->tail = no;
    return 0;
}
