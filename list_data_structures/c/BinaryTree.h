#pragma once

#include <stddef.h>

#include "ListOperations.h"

typedef struct BinaryTreeNode {
    void* item;
    struct BinaryTreeNode* left;
    struct BinaryTreeNode* right;
} BinaryTreeNode;

typedef struct {
    comparator comparator;
    freer freer;
    BinaryTreeNode* root;
} BinaryTree;

BinaryTree* BinaryTree_Create(comparator, freer);
ListOpResult BinaryTree_Insert(BinaryTree*, void*);
ListOpResult BinaryTree_Delete(BinaryTree*, void*);
void* BinaryTree_Search(const BinaryTree*, const void*);
ListOpResult BinaryTree_Enumerate(const BinaryTree*, item_handler);
void BinaryTree_Destroy(BinaryTree*);
