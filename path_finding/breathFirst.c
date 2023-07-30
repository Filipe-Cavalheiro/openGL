#include "../data_structures/linkedList.h"
#include "breathFirst.h"

#define MAX_ITERATIONS 1000

void breathFirstSearch(int startPos[2], int finishPos[2]){    
    linkedList queue = makeLinkedList();
    append(queue, startPos);
    printf("number: %d\n", *(int *)getElem(getHead(queue)));
}

