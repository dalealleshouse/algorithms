#pragma once

#include <stddef.h>

#include "ListOperations.h"

typedef struct BinaryTreeNode {
    size_t size;
    struct BinaryTreeNode* left;
    struct BinaryTreeNode* right;
    struct BinaryTreeNode* parent;
    void* item;
} BinaryTreeNode;

typedef struct {
    size_t n;
    comparator comparator;
    BinaryTreeNode* root;
} BinaryTree;

BinaryTree* BinaryTree_Create(comparator);
ListOpResult BinaryTree_Insert(BinaryTree*, void*);
ListOpResult BinaryTree_Enumerate(const BinaryTree*, item_handler);
void* BinaryTree_Delete(BinaryTree*, void*);
void* BinaryTree_Search(const BinaryTree*, const void*);
void* BinaryTree_Min(const BinaryTree*);
void* BinaryTree_Max(const BinaryTree*);
void* BinaryTree_Predecessor(const BinaryTree*, const void*);
void* BinaryTree_Successor(const BinaryTree*, const void*);
void* BinaryTree_Select(const BinaryTree*, const size_t);
size_t BinaryTree_Rank(const BinaryTree*, const void*);
void BinaryTree_Destroy(BinaryTree*, freer);
