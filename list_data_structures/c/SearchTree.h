#pragma once

#include <stddef.h>

#include "ListOperations.h"

typedef struct SearchTreeNode {
    size_t size;
    struct SearchTreeNode* left;
    struct SearchTreeNode* right;
    struct SearchTreeNode* parent;
    void* item;
} SearchTreeNode;

typedef struct {
    size_t n;
    comparator comparator;
    SearchTreeNode* root;
} SearchTree;

SearchTree* SearchTree_Create(comparator);
ListOpResult SearchTree_Insert(SearchTree*, void*);
ListOpResult SearchTree_Delete(SearchTree*, void*);
ListOpResult SearchTree_Enumerate(const SearchTree*, item_handler);
void* SearchTree_Search(const SearchTree*, const void*);
void* SearchTree_Min(const SearchTree*);
void* SearchTree_Max(const SearchTree*);
void* SearchTree_Predecessor(const SearchTree*, const void*);
void* SearchTree_Successor(const SearchTree*, const void*);
void* SearchTree_Select(const SearchTree*, const size_t);
size_t SearchTree_Rank(const SearchTree*, const void*);
void SearchTree_Destroy(SearchTree*, freer);
