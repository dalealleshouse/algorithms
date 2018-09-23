#pragma once

#include <stddef.h>

#include "ListOperations.h"

typedef int (*comparator)(const void* x, const void* y);

typedef struct BinaryTreeNode {
    void* item;
    struct BinaryTreeNode* left;
    struct BinaryTreeNode* right;
} BinaryTreeNode;

typedef struct {
    comparator comparator;
    BinaryTreeNode* root;
} BinaryTree;

BinaryTree* BinaryTree_Create(comparator);
ListOpResult BinaryTree_Insert(BinaryTree*, void*);
ListOpResult BinaryTree_Delete(BinaryTree*, void*);
void BinaryTree_Destroy(BinaryTree*);
