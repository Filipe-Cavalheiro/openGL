#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "linkedList.h"

//This ADT will be used to hold, give and change the information of a linked list
struct _linkedList{
    node head, tail;
    int nElems;
};

linkedList makeLinkedList(){
    linkedList list = (linkedList)malloc(sizeof(struct _linkedList));
    if (list == NULL)
        return NULL;
    list->head = NULL;
    list->tail = NULL;
    list->nElems = 0;
    return list;
}

void destroyList(linkedList list){
    node aux = list->head;
    while (aux != NULL){
        list->head = nextNode(aux);
        destroyNode(aux);
        aux = list->head;
    }
    free(list);
}

void destroyListAndElems(linkedList list, void (*destroy)(void *)){
    node aux = list->head;
    while (aux != NULL){
        list->head = nextNode(aux);
        destroyNodeAndElems(aux, destroy);
        aux = list->head;
    }
    free(list);
}

void* popList(linkedList list){
    if(list->head == NULL)
        return NULL;
    node aux = list->head;
    int* elem = getElem(aux);
    list->head = nextNode(aux);
    destroyNode(aux);
    --list->nElems;
    if (list->head != NULL) 
        setPrevNode(list->head, NULL);
    return (void*) elem;
}

void addHead(linkedList list, void *elem){
    node head = makeNode(elem);
    if (!list->nElems){
        list->tail = head;
        goto skip;
    }
    setNextNode(head, list->head);
    setPrevNode(list->head, head);

skip:
    list->head = head;
    ++list->nElems;
}

void append(linkedList list, void *elem){
    node tail = makeNode(elem);
    if (!list->nElems){
        list->head = tail;
    }
    else{
        setPrevNode(tail, list->tail);
        setNextNode(list->tail, tail);
    }
    list->tail = tail;
    ++list->nElems;
}

node locateIndex(linkedList list, int index){
    int d = (list->nElems / 2 + 1) - index;
    node aux = (d > 0) ? list->head : list->tail;
    if (d > 0){
        for (int i = 0; i < index; ++i)
            aux = nextNode(aux);
    }
    else{
        for (int i = list->nElems; i > index; --i)
            aux = prevNode(aux);
    }
    return aux;
}

void insert_ll(linkedList list, void *elem, int index){
    if (!index){
        addHead(list, elem);
        return;
    }

    node nNode = makeNode(elem);
    node aux = locateIndex(list, index);

    setNextNode(nNode, aux);
    setNextNode(prevNode(aux), nNode);
    setPrevNode(nNode, prevNode(aux));
    setPrevNode(aux, nNode);

    ++list->nElems;
}

void moveToTail(linkedList list, node tail){
    if (tail == list->tail)
        return;

    node next = nextNode(tail);
    node prev = prevNode(tail);

    if (prev == NULL)
    {
        setPrevNode(next, prev);
        list->head = next;
    }
    else
    {
        setNextNode(prev, next);
        setPrevNode(next, prev);
    }

    setNextNode(tail, NULL);

    if (!list->nElems)
    {
        list->head = tail;
        list->tail = tail;
        setPrevNode(tail, NULL);
        return;
    }

    setPrevNode(tail, list->tail);
    setNextNode(list->tail, tail);

    list->tail = tail;
}

int sizeList(linkedList list){
    return list->nElems;
}

node getHead(linkedList list){
    return (list->head);
}

node removeIndex(linkedList list, int index){
    node aux = getHead(list);
    for(int i = 0; i < index; ++i){
        if(aux == NULL){
            return NULL;
        }
        aux = nextNode(aux);
    }

    node parent = prevNode(aux);
    node child = nextNode(aux);
    setPrevNode(child, parent);
    setNextNode(parent, child);
    
    return aux;
}

node existElem(linkedList list, char *name, char *(*getName)(void *)){
    node aux = list->head;
    node aux2 = list->tail;
    for (int i = 0; i < list->nElems / 2 + 1; ++i){
        if (!strcmp(name, getName(getElem(aux))))
            return aux;
        else if (!strcmp(name, getName(getElem(aux2))))
            return aux2;

        aux = nextNode(aux);
        aux2 = prevNode(aux2);
    }
    return NULL;
}

void print_ll(linkedList list){
    node aux = list->head;
    while (aux != NULL){
        printf("%d ", *(int*)getElem(aux));
        aux = nextNode(aux);
    }
}

