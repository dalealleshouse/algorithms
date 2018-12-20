#pragma once

#include <stddef.h>

#include "ListOperations.h"

typedef enum { INVALID = 0, BLACK = 1, RED = 2 } COLOR;

typedef struct BinaryTreeNode {
    struct BinaryTreeNode* left;
    struct BinaryTreeNode* right;
    struct BinaryTreeNode* parent;
    void* payload;
    size_t size;
    COLOR color;
} BinaryTreeNode;

typedef struct {
    comparator comparator;
    BinaryTreeNode* root;
    size_t n;
} BinaryTree;

BinaryTreeNode sentinel;

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

// Replace the insert and delete methods above with these to have a self
// balancing red black tree
ListOpResult RedBlackTree_Insert(BinaryTree*, void*);
void* RedBlackTree_Delete(BinaryTree*, void*);
