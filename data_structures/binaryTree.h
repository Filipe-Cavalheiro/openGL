#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <stdio.h>
#include <stdlib.h>
typedef struct _binaryTree *binaryTree;
#include "binaryTree.c"

binaryTree makeBinaryTree(binaryTree parent, int data);

// Function to insert a new node into the tree
binaryTree insert_bt(binaryTree root, int data);

// Function to find the minimum value node in a tree
binaryTree findMin(binaryTree node);

// Function to remove a node from the tree
binaryTree removeNode(binaryTree root, int data);

// Function to deallocate memory of the tree
void freeTree(binaryTree root);

void printTree(binaryTree root, int level);
#endif /* HASHTABLE_H */
