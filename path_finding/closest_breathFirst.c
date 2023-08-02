#include "../data_structures/hashtable.h"
#include "../data_structures/linkedList.h"
#include "closest_breathFirst.h"

int checkPos(int* position, int* matrix){
    if(position[0] < 0 || position[1] < 0)
        return 1;
    if(position[0] > 5 || position[1] > 5)
        return 1;
    return matrix[position[0] + (position[1] * 6)];
}

linkedList closest_path(int* matrix, hashtable ht,int* key, int* startPos){
    linkedList path = makeLinkedList();
    int* tmp;
    while((key[0] != startPos[0]) || (key[1] != startPos[1])){
        tmp = get(ht, key);
        key[0] = tmp[0];
        key[1] = tmp[1];
        matrix[key[0] + (key[1] * 6)] = 5;
        append(path, key);
    }
    return path;
}

linkedList breathFirstSearch(int startPos[2], int finishPos[2], int* matrix){
    linkedList queue = makeLinkedList();
    if(queue == NULL){
        perror("could not create list\n");
        return NULL;
    }
    int* elem = (int*)malloc(sizeof(int) * 2);
    elem[0] = startPos[0];
    elem[1] = startPos[1];
    append(queue, elem);

    hashtable table = makeHashtable();
    if (table == NULL) {
        perror("Failed to allocate memory for a hashtable\n");
        exit(EXIT_FAILURE);
    }
    insert_ht(table, elem, elem);

    int* pos;
    while(sizeList(queue) != 0){
        pos = (int *)popList(queue);

        for(int i = -1; i < 2; ++i){
            for(int j = -1; j < 2; ++j){
                elem = (int*)malloc(sizeof(int) * 2);
                if(elem == NULL){
                    perror("could not create elem\n");
                    return NULL;
                }
                elem[1] = pos[1] + i;
                elem[0] = pos[0] + j;
                int typePos = checkPos(elem, matrix);
                if((typePos == 0) || (typePos == 3)){
                    matrix[elem[0] + elem[1] * 6] = 4;
                    append(queue, elem);
                    insert_ht(table, elem, pos);
                    if((elem[0] == finishPos[0]) && (elem[1] == finishPos[1])){
                        free(pos);
                        return closest_path(matrix, table, elem, startPos);
                    }
                }
            }
        }
        free(pos);
    }
    return NULL;
}
