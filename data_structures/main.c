#include "mAryTree.h"
#include "minHeap.h"

int main(){
    mAryTreeNode root = makeMAryTreeNode(10);
    print_minHeap(root);
    insert_minHeap(root, 5);
    print_minHeap(root);
    insert_minHeap(root, 3);
    print_minHeap(root);
    insert_minHeap(root, 8);
    print_minHeap(root);
    insert_minHeap(root, 2);
    print_minHeap(root);
    insert_minHeap(root, 4);
    print_minHeap(root);
    insert_minHeap(root, 10);
    print_minHeap(root);
    insert_minHeap(root, 131);
    print_minHeap(root);
    insert_minHeap(root, 30);
    print_minHeap(root);
    insert_minHeap(root, 10);
    print_minHeap(root);
    insert_minHeap(root, 0);
    print_minHeap(root);
    insert_minHeap(root, 564);
    print_minHeap(root);
    insert_minHeap(root, 74);
    print_minHeap(root);
    insert_minHeap(root, 34);
    print_minHeap(root);
    insert_minHeap(root, 7);
    print_minHeap(root);
    insert_minHeap(root, 12);
    print_minHeap(root);
    insert_minHeap(root, 2);
    print_minHeap(root);

    return 0;
}
