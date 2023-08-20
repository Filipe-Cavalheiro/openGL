#include "minHeap.h"

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void insert_minHeap(mAryTreeNode parent, int elem){
    linkedList queue = makeLinkedList();
    linkedList childs = getChildren(parent);
    node aux = getHead(childs);
    do{
        for(int i = 0; i < 2; ++i){
            if(aux == NULL){
                makeInsert_mAry(parent, elem);
                return;
            }
            append(queue, aux);
            aux = nextNode(aux);
        }
        aux = (node)popList(queue);
        parent = getElem(aux);
        childs = getChildren(parent);
        aux = getHead(childs);
    }while(sizeList(queue) != 0);
    return;
}

void print_minHeap(mAryTreeNode parent){ 
    printf("Heap:\n");
    printf("%d\n", getElem_mAry(parent));
    printChilldren(parent);

    linkedList queue = makeLinkedList();
    linkedList childs = getChildren(parent);
    node aux = getHead(childs);
    int count = 0, sum = 1;
    do{
        for(int i = 0; i < 2; ++i){
            if(aux == NULL){
                printf("\n");
                return;
            }
            append(queue, aux);
            aux = nextNode(aux);
        }
        aux = (node)popList(queue);
        parent = getElem(aux);
        childs = getChildren(parent);
        aux = getHead(childs);
        ++count;
        if(count == sum){
            printf("\n");
            count = 0;
            sum *= 2;
        }
        printChilldren(parent);
    }while(sizeList(queue) != 0);
}
/*
   int delete(minHeap *heap, int item) {
   if (heap->size == 0) {
   printf("Heap is empty. Cannot delete.\n");
   return -1;
   }

   int index = -1;
   for (int i = 0; i < heap->size; i++) {
   if (heap->data[i] == item) {
   index = i;
   break;
   }
   }

   if (index == -1) {
   printf("Item not found in heap.\n");
   return -1;
   }

   int deletedValue = heap->data[index];
   heap->data[index] = heap->data[heap->size - 1];
   heap->size--;

   heapify(heap, index);
   return deletedValue;
   }*/
/*
   int extract(minHeap *heap) {
   if (heap->size == 0) {
   printf("Heap is empty. Cannot extract.\n");
   return -1;
   }

   int extractedValue = heap->data[0];
   heap->data[0] = heap->data[heap->size - 1];
   heap->size--;

   heapify(heap, 0);
   return extractedValue;
   }

   bool isEmpty(minHeap *heap) {
   return heap->size == 0;
   }

   int size(minHeap *heap) {
   return heap->size;
   }*/
