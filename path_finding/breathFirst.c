#include "../data_structures/linkedList.h"
#include "../data_structures/node.h"
#include "breathFirst.h"

int checkPos(int* position, int* matrix){
    if(position[0] < 0 || position[1] < 0)
        return 1;
    if(position[0] > 5 || position[1] > 5)
        return 1;
    return matrix[position[0] + (position[1] * 6)];
}

void breathFirstSearch(int startPos[2], int finishPos[2], int* matrix){
    linkedList queue = makeLinkedList();
    if(queue == NULL){
        perror("could not create list\n");
        return;
    }
    int* elem = (int*)malloc(sizeof(int) * 2);
    elem[0] = startPos[0];
    elem[1] = startPos[1];
    append(queue, elem);

    int* pos;
    while(sizeList(queue) != 0){
        pos = (int *)popList(queue);

        if((pos[0] == finishPos[0]) && (pos[1] == finishPos[1])){
            matrix[startPos[0] + (startPos[1] * 6)] = 2;
            matrix[finishPos[0] + (finishPos[1] * 6)] = 3;
            free(pos);
            return;
        }
        matrix[pos[0] + pos[1] * 6] = 4;

        for(int i = -1; i < 2; ++i){
            for(int j = -1; j < 2; ++j){
                elem = (int*)malloc(sizeof(int) * 2);
                if(elem == NULL){
                    perror("could not create elem\n");
                    return;
                }
                elem[1] = pos[1] + i;
                elem[0] = pos[0] + j;
                int typePos = checkPos(elem, matrix);
                if((typePos == 0) || (typePos == 3)){
                    append(queue, elem);
                }
            }
        }
        free(pos);
    }
    matrix[startPos[0] + (startPos[1] * 6)] = 2;
    matrix[finishPos[0] + (finishPos[1] * 6)] = 3;
    return;
}
