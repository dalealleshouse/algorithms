// Copyright 2020 Hideous Humpback Freak https://hideoushumpbackfreak.com/
#pragma once

#include <stddef.h>

#include "comparators.h"
#include "result_code.h"

typedef enum { kInvalid = 0, kBlack = 1, kRed = 2 } Color;
typedef void (*item_handler)(void* x);

typedef struct BinaryTreeNode {
  struct BinaryTreeNode* left;
  struct BinaryTreeNode* right;
  struct BinaryTreeNode* parent;
  void* payload;
  size_t size;
  Color color;
} BinaryTreeNode;

typedef struct {
  comparator comparator;
  BinaryTreeNode* root;
  size_t n;
} BinaryTree;

BinaryTreeNode kNullNode;

ResultCode BinaryTree_Create(comparator, BinaryTree**);
ResultCode BinaryTree_Insert(BinaryTree*, void*);
ResultCode BinaryTree_Enumerate(const BinaryTree*, item_handler);
ResultCode BinaryTree_Delete(BinaryTree*, void*, void**);
ResultCode BinaryTree_Search(const BinaryTree*, const void*, void**);
ResultCode BinaryTree_Min(const BinaryTree*, void**);
ResultCode BinaryTree_Max(const BinaryTree*, void**);
ResultCode BinaryTree_Predecessor(const BinaryTree*, const void*, void**);
ResultCode BinaryTree_Successor(const BinaryTree*, const void*, void**);
ResultCode BinaryTree_Select(const BinaryTree*, const size_t, void**);
ResultCode BinaryTree_Rank(const BinaryTree*, const void*, size_t*);
void BinaryTree_Destroy(BinaryTree*, freer);

// Replace the insert methods above with these to have a self balancing red
// black tree
ResultCode RedBlackTree_Insert(BinaryTree*, void*);
ResultCode BinaryTree_RotateLeft(BinaryTree*, const void*);
ResultCode BinaryTree_RotateRight(BinaryTree*, const void*);
