#ifndef NODE_H
#define NODE_H

typedef struct _node *node;
#include "node.c"

/***********************************************
newNode - creates new node.
Parameters:
    elem - element of the node
Retorno: created node
Pre-condicoes: list != NULL
***********************************************/
node makeNode(void *elem);

/***********************************************
destroyNode - Frees the memory occupied by the node.
Parameters:
    aux - element of the node
Retorno:
Pre-condicoes: aux != NULL
***********************************************/
void destroyNode(node aux);

/***********************************************
destroyNode - Frees the memory occupied by the instance of the structure associated to the node and the node.
Parameters:
    aux - element of the node
Retorno:
Pre-condicoes: aux != NULL
***********************************************/
void destroyNodeAndElems(node aux, void (*destroy)(void *));

/***********************************************
nextNode - gives the next node.
Parameters:
    aux - node to take as reference
Retorno: next node
Pre-condicoes: aux != NULL
***********************************************/
node nextNode(node aux);

/***********************************************
prevNode - gives the previous node.
Parameters:
    aux - node to take as reference
Retorno: the node before
Pre-condicoes: aux != NULL
***********************************************/
node prevNode(node aux);

/***********************************************
getElem - gets the element from the node.
Parameters:
    aux - node to take as reference
Retorno: Element of the node
Pre-condicoes: aux != NULL
***********************************************/
void *getElem(node aux);

/***********************************************
setNextNode - changes the forward node this node points to.
Parameters:
    aux - node to take as reference
Retorno:
Pre-condicoes: aux != NULL
***********************************************/
void setNextNode(node aux, node aux2);

/***********************************************
setPrevNode - changes the backward node this node points to.
Parameters:
    aux - node to take as reference
Retorno:
Pre-condicoes: aux != NULL
***********************************************/
void setPrevNode(node aux, node aux2);

#endif
