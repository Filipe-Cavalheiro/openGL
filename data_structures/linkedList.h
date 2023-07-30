#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "node.h"
typedef struct _linkedList *linkedList;
#include "linkedList.c"

/***********************************************
newList - Creation of the structure instance associated to a sequence.
Parameters: None
Return: pointer of linked list
pre-conditions:
***********************************************/
linkedList makeLinkedList();

/***********************************************
destroyList - Frees the memory occupied by the instance of the structure associated to the sequence.
Parameters:
	list - the list to be destroyed
Return:
pre-conditions: list != NULL
***********************************************/
void destroyList(linkedList list);

/***********************************************
destroyListAndElems - Frees the memory occupied by the instance of the structure associated to the sequence and its elements.
Parameters:
	list - sequence to destroy
	destroy - function to destroy the elements in the sequence.
Return:
pre-conditions: list != NULL
***********************************************/
void destroyListAndElems(linkedList list, void (*destroy)(void *));

/***********************************************
addHead - Adds the given element too the firts position of the sequence.
Parameters:
	list - sequence
	head - element's pointer
Return:
pre-conditions: list != NULL && head != NULL
***********************************************/
void addHead(linkedList list, void *elem);

/***********************************************
append - Adds the given element too the last position of the sequence.
Parameters:
	list - sequence
	tail - element's pointer
Return:
pre-conditions: list != NULL && tail != NULL
***********************************************/
void append(linkedList list, void *elem);

/***********************************************
insert - Adds the given element in the i-th sequence position.
Parameters:
	list - sequence
	newNode - element's pointer
	index - position in the sequence
Return:
Pre-conditions: list != NULL && index>0 && index<= sizeCertified
***********************************************/
void insert(linkedList list, void *elem, int index);

/***********************************************
moveToTail - Moves the given element too the last position of the sequence.
Parameters:
	list - sequence
	tail - element's pointer
Return:
pre-conditions: list != NULL && tail != NULL
***********************************************/
void moveToTail(linkedList list, node tail);

/***********************************************
sizeList - Checks the number of elements in the sequence.
Parameters:
	list - sequence
Return: number of elements in the sequence
Pre-conditions: list != NULL
***********************************************/
int sizeList(linkedList list);

/***********************************************
getHead - obtains the head of the given list.
Parameters:
	list - sequence
Return:
pre-conditions: list != NULL
***********************************************/
node getHead(linkedList list);

/***********************************************
existElem - return if a given element exits.
Parameters:
	list - sequence
	name - name of the element
	getName - fuction to obtain the name of elements in the sequence 
Return: iterator of a sequence
pre-conditions: list != NULL
***********************************************/
node existElem(linkedList list, char *name, char *(*getName)(void *));

#endif // LINKEDLIST_H
