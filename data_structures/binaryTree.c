#include "binaryTree.h"

// Define the binary tree node structure
struct _binaryTree {
    int data;
    struct _binaryTree* parent;
    struct _binaryTree* left;
    struct _binaryTree* right;
} ;

// Function to create a new node
binaryTree makeBinaryTree(binaryTree parent, int data) {
    binaryTree newNode = (binaryTree)malloc(sizeof(struct _binaryTree));
    if (newNode == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->parent = parent;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

int getElem_bt(binaryTree tree){
    return tree->data;
}

// Function to insert a new node into the tree
binaryTree insert_bt(binaryTree root, int data) {
    if (root == NULL) {
        return makeBinaryTree(root, data);
    }

    if (data <= root->data) {
        root->left = insert_bt(root->left, data);
    } else if (data > root->data) {
        root->right = insert_bt(root->right, data);
    }

    return root;
}

// Function to find the minimum value node in a tree
binaryTree findMin(binaryTree node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

// Function to find the minimum value node in a tree
binaryTree findMax(binaryTree node) {
    while (node->right != NULL) {
        node = node->right;
    }
    return node;
}

// Function to remove a node from the tree
binaryTree removeNode(binaryTree root, int data) {
    if (root == NULL) {
        return root;
    }

    if (data <= root->data) {
        root->left = removeNode(root->left, data);
    } else if (data > root->data) {
        root->right = removeNode(root->right, data);
    } else {
        // binaryTree with only one child or no child
        if (root->left == NULL) {
            binaryTree temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            binaryTree temp = root->left;
            free(root);
            return temp;
        }

        // binaryTree with two children: Get the inorder successor (smallest in the right subtree)
        binaryTree temp = findMin(root->right);

        // Copy the inorder successor's content to this node
        root->data = temp->data;

        // Delete the inorder successor
        root->right = removeNode(root->right, temp->data);
    }

    return root;
}

// Function to deallocate memory of the tree
void freeTree(binaryTree root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

void printTree(binaryTree root, int level) {
    if (root == NULL) {
        return;
    }

    printTree(root->right, level + 1);

    printf("%d\n", root->data);

    printTree(root->left, level + 1);
}

