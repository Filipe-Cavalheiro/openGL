#ifndef HASHTABLE_H
#define HASHTABLE_H

#define HASHTABLE_SIZE 100

typedef struct _hashtable *hashtable;
typedef struct _hashNode *hashNode;
#include "hashtable.c"

// Function to create a new hashtable
hashtable makeHashtable();

// Function to calculate the hash value for a given key
unsigned int hash(int* key);

// Function to insert a key-value pair into the hashtable
void insert_ht(hashtable ht, int* key, int* value);

// Function to retrieve the value associated with a key from the hashtable
int* get(hashtable ht, int* key);

// Function to remove a key-value pair from the hashtable
void removeKey(hashtable ht, int* key);

// Function to free the memory occupied by the hashtable
void freeHashtable(hashtable ht);

#endif /* HASHTABLE_H */
