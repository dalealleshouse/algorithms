#pragma once

#include <stddef.h>

typedef enum {
    BinaryTree_NullParamter = -2,
    BinaryTree_BadMalloc = -1,
    BinaryTree_Success = 0
} BinaryTreeResult;

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
BinaryTreeResult BinaryTree_Insert(BinaryTree*, void*);
BinaryTreeResult BinaryTree_Delete(BinaryTree*, void*);
void* BinaryTree_Search(BinaryTree*);
void BinaryTree_Destroy(BinaryTree*);
