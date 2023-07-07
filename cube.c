#include "cube.h"

typedef struct{
    void* data;
    void* next;
}No;

typedef struct{
    No *head;
    No *tail;
}LinkedList;

typedef struct{
    vec3 position;
    vec3 size;
}Cube;

No* makeNo(void* pointer2Struct){ 
    No *no = (No*)malloc(sizeof(No)); 
    no->data = pointer2Struct;
    no->next = NULL;
    return no;
}

LinkedList* makeLinkedList(){ 
    LinkedList *linkedList = (LinkedList*)malloc(sizeof(LinkedList));
    linkedList->head = NULL;
    linkedList->tail = NULL;
    return linkedList;
}

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
