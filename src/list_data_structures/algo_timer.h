#pragma once

#include <inttypes.h>
#include "./array.h"
#include "./binary_tree.h"
#include "./linked_list.h"
#include "./list_operations.h"

uintptr_t _enumerate_sum;

typedef enum {
  INSERT = 1,
  SEARCH = 2,
  ENUMERATE = 3,
  MAX = 4,
  PREDECESSOR = 5,
  SELECT = 6,
  RANK = 7
} Operation;

typedef enum {
  ARRAY = 1,
  SORTED_ARRAY = 2,
  LINKED_LIST = 3,
  LINKED_LIST_POOR_LOCALITY = 4,
  BINARY_TREE = 5,
  BINARY_TREE_UNBALANCED = 6,
  RED_BLACK_TREE = 7
} Structure;

typedef ListOpResult (*ListOp)(void*, uintptr_t);
typedef uintptr_t (*SearchOp)(const void*, const uintptr_t);
typedef void (*EnumerateOp)(const void*);
typedef uintptr_t (*MaxOp)(const void*);
typedef uintptr_t (*PredOp)(const void*, const uintptr_t);
typedef uintptr_t (*SelectOp)(const void*, const size_t);
typedef size_t (*RankOp)(const void*, const uintptr_t);

ListOp GetInsertOperation(Structure);
SearchOp GetSearchOperation(Structure);
EnumerateOp GetEnumerateOperation(Structure);

void* BuildEmptyDataStructure(Structure);
void* BuildDataStructure(Structure, size_t);
void DestroyStructure(Structure, void*);

int ptr_comparator(const void*, const void*);
double OperationTime(Operation, Structure, size_t);
