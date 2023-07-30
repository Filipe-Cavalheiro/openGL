#include <stdlib.h>

#include "node.h"

//This ADT will be used to hold, give and change the information of a node in the linked list
struct _node{
    void *elem;
    struct _node *previous;
    struct _node *next;
};

node makeNode(void *elem){
    node aux = (node)malloc(sizeof(struct _node));
    if (aux == NULL)
        return NULL;
    aux->elem = elem;
    aux->previous = NULL;
    aux->next = NULL;
    return aux;
}

void destroyNode(node aux){
    free(aux);
}

void destroyNodeAndElems(node aux, void (*destroy)(void *)){
    destroy(aux->elem);
    free(aux);
}

node nextNode(node aux){
    return aux->next;
}

node prevNode(node aux){
    return aux->previous;
}

void* getElem(node aux){
    return aux->elem;
}

void setNextNode(node aux, node aux2){
    aux->next = aux2;
}

void setPrevNode(node aux, node aux2){
    aux->previous = aux2;
}
