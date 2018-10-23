#pragma once

#include <inttypes.h>

#include "ListOperations.h"
#include "Array.h"
#include "LinkedList.h"
#include "BinaryTree.h"

typedef enum { INSERT = 1, SEARCH = 2, ENUMERATE = 3 } Operation;
typedef enum {
    ARRAY = 1,
    LINKED_LIST = 2,
    LINKED_LIST_POOR_LOCALITY = 3,
    BINARY_TREE = 4,
    BINARY_TREE_UNBALANCED = 5
} Structure;

typedef ListOpResult (*InsertOp)(void*, uintptr_t);
InsertOp GetInsertOperation(Structure str);

void* BuildEmptyDataStructure(Structure str);
void* BuildDataStructure(Structure str, size_t n);
void DestroyStructure(Structure, void*);

int ptr_comparator(const void*, const void*);
