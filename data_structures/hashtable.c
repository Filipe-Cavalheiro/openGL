#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

// hashNode structure for the hashtable
struct _hashNode{
    int key[2];
    int value[2];
    struct _hashNode* next;
};

// Hashtable structure
struct _hashtable {
    hashNode table[HASHTABLE_SIZE];
};

// Function to create a new e
hashtable makeHashtable() {
    hashtable ht = (hashtable)malloc(sizeof(struct _hashtable));
    if (ht == NULL) {
        perror("Failed to allocate memory for the hashtable");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        ht->table[i] = NULL;
    }

    return ht;
}

// Function to calculate the hash value for a given key
unsigned int hash(int* key){
    unsigned int hashVal = 31 * key[0] + key[1];
    return hashVal % HASHTABLE_SIZE;
}

// Function to insert a key-value pair into the hashtable
void insert_ht(hashtable ht, int* key, int* value) {
    unsigned int index = hash(key);
    hashNode newNode = (hashNode)malloc(sizeof(struct _hashNode));
    if (newNode == NULL) {
        perror("Failed to allocate memory for a new node");
        exit(EXIT_FAILURE);
    }

    newNode->key[0] = key[0];
    newNode->key[1] = key[1];
    newNode->value[0] = value[0];
    newNode->value[1] = value[1];
    newNode->next = NULL;

    if (ht->table[index] == NULL) {
        ht->table[index] = newNode;
    } else {
        // Handle hash collision by adding newNode at the end of the linked list
        hashNode current = ht->table[index];
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Function to retrieve the value associated with a key from the hashtable
int* get(hashtable ht, int* key) {
    unsigned int index = hash(key);
    hashNode current = ht->table[index];
    while (current != NULL) {
        if ((current->key[0] == key[0]) && (current->key[1] == key[1])) {
            return current->value;
        }
        current = current->next;
    }
    return NULL; // Key not found
}

// Function to remove a key-value pair from the hashtable
void removeKey(hashtable ht, int* key) {
    unsigned int index = hash(key);
    hashNode current = ht->table[index];
    hashNode prev = NULL;

    while (current != NULL) {
        if ((current->key[0] == key[0]) && (current->key[1] == key[1])) {
            if (prev == NULL) {
                ht->table[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Function to free the memory occupied by the hashtable
void freeHashtable(hashtable ht) {
    if (ht == NULL) {
        return;
    }

    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        hashNode current = ht->table[i];
        while (current != NULL) {
            hashNode temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }

    free(ht);
}

