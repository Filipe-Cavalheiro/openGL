#ifndef CLOSEST_BREATHFIRST_H
#define CLOSEST_BREATHFIRST_H 

#include "closest_breathFirst.c"

linkedList breathFirstSearch();
int checkPos(int* pos,int* matrix);
linkedList closest_path(int* matrix, hashtable ht,int* key, int* startPos);

#endif /* __CLOSEST_BREATHFIRST_H__ */
