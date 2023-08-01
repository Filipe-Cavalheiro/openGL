#ifndef BREATHFIRST_H
#define BREATHFIRST_H 

typedef struct _traceBack *traceBack;
#include "breathFirst.c"

void breathFirstSearch();
int checkPos(int* pos,int* matrix);

#endif /* __BREATHFIRST_H__ */
