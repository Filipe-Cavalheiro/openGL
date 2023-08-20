#ifndef MINHEAP_H
#define MINHEAP_H

#include "minHeap.c"

/***********************************************
insert_minHeap - insert a node in root with value elem.
Parameters: 
    parent - the one to be appended to
    elem - the value to be appended      
Return: nothing
pre-conditions: parent != NULL
***********************************************/
void insert_minHeap(mAryTreeNode parent, int elem);

/***********************************************
print_minHeap - print the entire heap.
Parameters: 
    root - heap to be printed    
Return: nothing
pre-conditions: root != NULL
***********************************************/
void print_minHeap(mAryTreeNode root);

#endif // MARYTREE_H

