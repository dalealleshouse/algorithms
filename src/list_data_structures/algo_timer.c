// Copyright 2020 Dale Alleshouse
#include "algo_timer.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../utils/math.h"
#include "./sorted_array.h"

void* BuildkEmptyDataStructure(Structure str) {
  switch (str) {
    case kArray:
    case kSortedArray:
      return Array_Create(PIntComparator, sizeof(uintptr_t));
    case kLinkedList:
    case kLinkedListPoorLocality:
      return LinkedList_Create(NULL, ptr_comparator);
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      return BinaryTree_Create(ptr_comparator);
  }

  return NULL;
}

void* BuildDataStructure(Structure str, size_t n) {
  unsigned int seed = time(NULL);
  void* ds = BuildkEmptyDataStructure(str);
  ListOp op = GetInsertOperation(str);
  if (ds == NULL) return NULL;

  switch (str) {
    case kArray:
    case kLinkedList:
    case kBinaryTree:
    case kRedBlackTree:
      for (size_t i = 0; i < n; i++) op(ds, rand_r(&seed));
      break;
    case kSortedArray:
      for (size_t i = 0; i < n; i++) op(ds, rand_r(&seed));

      Array* arr = (Array*)ds;
      qsort(arr->array, arr->n, arr->item_size, arr->comparator);
      break;
    case kLinkedListPoorLocality:
      for (size_t i = 0; i < n * 4; i++) {
        void* temp = malloc(10000);
        op(ds, rand_r(&seed));
        free(temp);
      }

      LinkedList* list = (LinkedList*)ds;
      for (size_t i = 0; i < n * 3; i++) {
        LinkedList_DeleteAt(list, rand_r(&seed) % list->size);
      }
      break;
    case kBinaryTreeUnbalanced:
      for (size_t i = 1; i <= n; i++) op(ds, i);
      break;
    default:
      return NULL;
  }

  return ds;
}

void DestroyStructure(Structure str, void* structure) {
  switch (str) {
    case kArray:
    case kSortedArray:
      Array_Destroy(structure);
      break;
    case kLinkedList:
    case kLinkedListPoorLocality:
      LinkedList_Destroy(structure);
      break;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      BinaryTree_Destroy(structure, NULL);
      break;
  }
}

int ptr_comparator(const void* x, const void* y) {
  if (x == y) return 0;

  uintptr_t _x = (uintptr_t)x;
  uintptr_t _y = (uintptr_t)y;

  if (_x == _y) return 0;

  if (_x < _y) return -1;

  return 1;
}

ListOpResult Array_InsertOp(void* array, uintptr_t item) {
  return Array_Insert(array, &item);
}

ListOpResult LinkedList_InsertOp(void* list, uintptr_t item) {
  LinkedList* list_t = (LinkedList*)list;
  return LinkedList_InsertAt(list, (void*)item, list_t->size);
}

ListOpResult BinaryTree_InsertOp(void* tree, uintptr_t item) {
  return BinaryTree_Insert(tree, (void*)item);
}

ListOpResult RedBlackTree_InsertOp(void* tree, uintptr_t item) {
  return RedBlackTree_Insert(tree, (void*)item);
}

ListOp GetInsertOperation(Structure str) {
  switch (str) {
    case kArray:
    case kSortedArray:
      return Array_InsertOp;
    case kLinkedList:
    case kLinkedListPoorLocality:
      return LinkedList_InsertOp;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
      return BinaryTree_InsertOp;
    case kRedBlackTree:
      return RedBlackTree_InsertOp;
  }

  return NULL;
}

uintptr_t Array_SearchOp(const void* array, const uintptr_t item) {
  void* result = Array_Search(array, &item);

  if (result == NULL) return 0;

  return *(uintptr_t*)result;
}

uintptr_t SortedArray_SearchOp(const void* array, const uintptr_t item) {
  void* result = SortedArray_Search(array, &item);

  if (result == NULL) return 0;

  return *(uintptr_t*)result;
}

uintptr_t LinkedList_SearchOp(const void* list, const uintptr_t item) {
  return (uintptr_t)LinkedList_Search(list, (void*)item);
}

uintptr_t BinaryTree_SearchOp(const void* tree, const uintptr_t item) {
  return (uintptr_t)BinaryTree_Search(tree, (void*)item);
}

SearchOp GetSearchOperation(Structure str) {
  switch (str) {
    case kArray:
      return Array_SearchOp;
    case kSortedArray:
      return SortedArray_SearchOp;
    case kLinkedList:
    case kLinkedListPoorLocality:
      return LinkedList_SearchOp;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      return BinaryTree_SearchOp;
  }

  return NULL;
}

void Array_EnumerateHandler(void* item) { _enumerate_sum += *(uintptr_t*)item; }

void Array_EnumerateOp(const void* array) {
  Array_Enumerate(array, Array_EnumerateHandler);
}

void ListAndTree_EnumerateHandler(void* item) {
  _enumerate_sum += (uintptr_t)item;
}

void LinkedList_EnumerateOp(const void* list) {
  LinkedList_Enumerate(list, ListAndTree_EnumerateHandler);
}

void BinaryTree_EnumerateOp(const void* tree) {
  BinaryTree_Enumerate(tree, ListAndTree_EnumerateHandler);
}

EnumerateOp GetEnumerateOperation(Structure str) {
  switch (str) {
    case kArray:
    case kSortedArray:
      return Array_EnumerateOp;
    case kLinkedList:
    case kLinkedListPoorLocality:
      return LinkedList_EnumerateOp;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      return BinaryTree_EnumerateOp;
  }

  return NULL;
}

uintptr_t SortedArray_MaxOp(const void* array) {
  return (uintptr_t)SortedArray_Max(array);
}

uintptr_t BinaryTree_MaxOp(const void* tree) {
  return (uintptr_t)BinaryTree_Max(tree);
}

MaxOp GetMaxOperation(Structure str) {
  switch (str) {
    case kSortedArray:
      return SortedArray_MaxOp;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      return BinaryTree_MaxOp;
    default:
      return NULL;
  }
}

uintptr_t SortedArray_PredecessorOp(const void* array, const uintptr_t item) {
  return (uintptr_t)SortedArray_Predecessor(array, &item);
}

uintptr_t BinaryTree_PredecessorOp(const void* tree, const uintptr_t item) {
  return (uintptr_t)BinaryTree_Predecessor(tree, &item);
}

PredOp GetPredOperation(Structure str) {
  switch (str) {
    case kSortedArray:
      return SortedArray_PredecessorOp;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      return BinaryTree_PredecessorOp;
    default:
      return NULL;
  }
}

uintptr_t SortedArray_SelectOp(const void* tree, const size_t item) {
  return (uintptr_t)SortedArray_Select(tree, item);
}

uintptr_t BinaryTree_SelectOp(const void* tree, const size_t item) {
  return (uintptr_t)BinaryTree_Select(tree, item);
}

SelectOp GetSelectOperation(Structure str) {
  switch (str) {
    case kSortedArray:
      return SortedArray_SelectOp;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      return BinaryTree_SelectOp;
    default:
      return NULL;
  }
}

size_t SortedArray_RankOp(const void* tree, const size_t item) {
  return (uintptr_t)SortedArray_Rank(tree, &item);
}

size_t BinaryTree_RankOp(const void* tree, const size_t item) {
  return (uintptr_t)BinaryTree_Rank(tree, &item);
}

RankOp GetRankOp(Structure str) {
  switch (str) {
    case kSortedArray:
      return SortedArray_RankOp;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      return BinaryTree_RankOp;
    default:
      return NULL;
  }
}

double OperationTime(Operation op, Structure st, size_t n) {
  unsigned int seed = time(NULL);
  clock_t t;
  void* ds;

  switch (op) {
    case kInsert:
      // In the case of insert, n is ignored
      ds = BuildkEmptyDataStructure(st);
      ListOp op = GetInsertOperation(st);
      if (ds == NULL || op == NULL) return -1;

      // Timed operation
      t = clock();
      for (size_t i = 0; i < n; i++) op(ds, rand_r(&seed));
      t = clock() - t;
      break;
    case kSearch:
      ds = BuildDataStructure(st, n);
      SearchOp s_op = GetSearchOperation(st);
      if (ds == NULL || s_op == NULL) return -1;

      t = clock();
      for (size_t i = 0; i < n; i++) s_op(ds, rand_r(&seed));
      t = clock() - t;
      break;
    case kEnumerate:
      ds = BuildDataStructure(st, n);
      EnumerateOp e_op = GetEnumerateOperation(st);
      _enumerate_sum = 0;

      t = clock();
      e_op(ds);
      t = clock() - t;
      break;
    case kMax:
      ds = BuildDataStructure(st, n);
      MaxOp m_op = GetMaxOperation(st);

      t = clock();
      for (size_t i = 0; i < n; i++) m_op(ds);
      t = clock() - t;
      break;
    case kPredecessor:
      ds = BuildDataStructure(st, n);
      PredOp p_op = GetPredOperation(st);

      t = clock();
      for (size_t i = 0; i < n; i++) p_op(ds, rand_r(&seed));
      t = clock() - t;
      break;
    case kSelect:
      ds = BuildDataStructure(st, n);
      SelectOp se_op = GetSelectOperation(st);

      t = clock();
      for (size_t i = 0; i < n; i++) se_op(ds, rand_r(&seed) % n);
      t = clock() - t;
      break;
    case kRank:
      ds = BuildDataStructure(st, n);
      RankOp r_op = GetRankOp(st);

      t = clock();
      for (size_t i = 0; i < n; i++) r_op(ds, rand_r(&seed));
      t = clock() - t;
      break;
  }

  if (ds == NULL) return -1;

  DestroyStructure(st, ds);
  double time = ((double)t) / CLOCKS_PER_SEC;
  return time;
}
