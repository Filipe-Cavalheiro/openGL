#ifndef MARYTREE_H
#define MARYTREE_H

#include "linkedList.h"
typedef struct _mAryTreeNode *mAryTreeNode;
#include "mAryTree.c"

/***********************************************
makeMAryTree - Creation of the structure instance associated to a tree.
Parameters: val
Return: pointer of mAryTree
pre-conditions:
***********************************************/
mAryTreeNode makeMAryTreeNode(int val);

/***********************************************
makeInsert_mAry - turns elem in node and appends it.
Parameters: 
    parent - the one to be appended to
    elem - the value to be appended      
Return: created node
pre-conditions: parent != NULL
***********************************************/
mAryTreeNode makeInsert_mAry(mAryTreeNode parent, int elem);

/***********************************************
insert_mAry - appends node to parent.
Parameters: 
    parent - the one to be appended to
    child - the child to be appended     
Return: nothing
pre-conditions: parent != NULL
***********************************************/
void insert_mAry(mAryTreeNode parent, mAryTreeNode child);

/***********************************************
getElem - return elem of node
Parameters: 
    tree - 
Return: value
pre-conditions: tree != NULL
***********************************************/
int getElem_mAry(mAryTreeNode tree);

/***********************************************
getChildren - return list of node
Parameters: 
    tree -     
Return: list
pre-conditions: tree != NULL
***********************************************/
linkedList getChildren(mAryTreeNode tree);

/***********************************************
checkExixtence - verifie if certain index node exists
Parameters: 
    node - node to be checked
    index - index of the linkedlist to be checked
Return: 1 if true, 0 if false
pre-conditions: node != NULL, index >= 0
***********************************************/
int checkExixtence(mAryTreeNode node, int index);

/***********************************************
printChildren - print all children of passed parent.
Parameters: 
    parent -     
Return: nothing
pre-conditions: parent != NULL
***********************************************/
void printChilldren(mAryTreeNode parent);

#endif // MARYTREE_H

