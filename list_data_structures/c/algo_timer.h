#pragma once

#include <inttypes.h>

#include "Array.h"
#include "BinaryTree.h"
#include "LinkedList.h"
#include "ListOperations.h"

uintptr_t _enumerate_sum;

typedef enum { INSERT = 1, SEARCH = 2, ENUMERATE = 3 } Operation;
typedef enum {
    ARRAY = 1,
    LINKED_LIST = 2,
    LINKED_LIST_POOR_LOCALITY = 3,
    BINARY_TREE = 4,
    BINARY_TREE_UNBALANCED = 5
} Structure;

typedef ListOpResult (*ListOp)(void*, uintptr_t);
typedef uintptr_t (*SearchOp)(const void*, const uintptr_t);
typedef void (*EnumerateOp)(const void*);
ListOp GetInsertOperation(Structure);
SearchOp GetSearchOperation(Structure);
EnumerateOp GetEnumerateOperation(Structure);

void* BuildEmptyDataStructure(Structure);
void* BuildDataStructure(Structure, size_t);
void DestroyStructure(Structure, void*);

int ptr_comparator(const void*, const void*);
double OperationTime(Operation, Structure, size_t);
