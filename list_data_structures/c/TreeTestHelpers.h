#pragma once

#include <stddef.h>

#include "BinaryTree.h"

size_t NodeCount(BinaryTreeNode* node);
void TreeSizeEqualsNodeCount(BinaryTree* tree);
void NodeIsValid(BinaryTreeNode* node);
void TreePropertyHolds(BinaryTreeNode* node);
void TreeIsValid(BinaryTree* tree, size_t n);
BinaryTree* ManuallyGenerateValidTree();
