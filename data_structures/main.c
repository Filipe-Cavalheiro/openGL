#include "binaryTree.h"

int main(){
    binaryTree root = makeBinaryTree(NULL, 10);
    insert_bt(root, 5);
    insert_bt(root, 5);
    insert_bt(root, 1);
    insert_bt(root, 7);
    insert_bt(root, 26);
    printf("min: %d\n",getElem_bt(findMin(root)));
    printTree(root, 0);
    return 0;
}
