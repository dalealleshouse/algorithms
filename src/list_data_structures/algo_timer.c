// Copyright 2020 Dale Alleshouse
#include "./algo_timer.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../utils/math.h"
#include "./sorted_array.h"

void* BuildEmptyDataStructure(Structure str) {
  switch (str) {
    case ARRAY:
    case SORTED_ARRAY:
      return Array_Create(PIntComparator, sizeof(uintptr_t));
    case LINKED_LIST:
    case LINKED_LIST_POOR_LOCALITY:
      return LinkedList_Create(NULL, ptr_comparator);
    case BINARY_TREE:
    case BINARY_TREE_UNBALANCED:
    case RED_BLACK_TREE:
      return BinaryTree_Create(ptr_comparator);
  }

  return NULL;
}

void* BuildDataStructure(Structure str, size_t n) {
  void* ds = BuildEmptyDataStructure(str);
  ListOp op = GetInsertOperation(str);
  if (ds == NULL) return NULL;

  switch (str) {
    case ARRAY:
    case LINKED_LIST:
    case BINARY_TREE:
    case RED_BLACK_TREE:
      for (size_t i = 0; i < n; i++) op(ds, rand());
      break;
    case SORTED_ARRAY:
      for (size_t i = 0; i < n; i++) op(ds, rand());

      Array* arr = (Array*)ds;
      qsort(arr->array, arr->n, arr->item_size, arr->comparator);
      break;
    case LINKED_LIST_POOR_LOCALITY:
      for (size_t i = 0; i < n * 4; i++) {
        void* temp = malloc(10000);
        op(ds, rand());
        free(temp);
      }

      LinkedList* list = (LinkedList*)ds;
      for (size_t i = 0; i < n * 3; i++) {
        LinkedList_DeleteAt(list, rand() % list->size);
      }
      break;
    case BINARY_TREE_UNBALANCED:
      for (size_t i = 1; i <= n; i++) op(ds, i);
      break;
    default:
      return NULL;
  }

  return ds;
}

void DestroyStructure(Structure str, void* structure) {
  switch (str) {
    case ARRAY:
    case SORTED_ARRAY:
      Array_Destroy(structure);
      break;
    case LINKED_LIST:
    case LINKED_LIST_POOR_LOCALITY:
      LinkedList_Destroy(structure);
      break;
    case BINARY_TREE:
    case BINARY_TREE_UNBALANCED:
    case RED_BLACK_TREE:
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
    case ARRAY:
    case SORTED_ARRAY:
      return Array_InsertOp;
    case LINKED_LIST:
    case LINKED_LIST_POOR_LOCALITY:
      return LinkedList_InsertOp;
    case BINARY_TREE:
    case BINARY_TREE_UNBALANCED:
      return BinaryTree_InsertOp;
    case RED_BLACK_TREE:
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
    case ARRAY:
      return Array_SearchOp;
    case SORTED_ARRAY:
      return SortedArray_SearchOp;
    case LINKED_LIST:
    case LINKED_LIST_POOR_LOCALITY:
      return LinkedList_SearchOp;
    case BINARY_TREE:
    case BINARY_TREE_UNBALANCED:
    case RED_BLACK_TREE:
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
    case ARRAY:
    case SORTED_ARRAY:
      return Array_EnumerateOp;
    case LINKED_LIST:
    case LINKED_LIST_POOR_LOCALITY:
      return LinkedList_EnumerateOp;
    case BINARY_TREE:
    case BINARY_TREE_UNBALANCED:
    case RED_BLACK_TREE:
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
    case SORTED_ARRAY:
      return SortedArray_MaxOp;
    case BINARY_TREE:
    case BINARY_TREE_UNBALANCED:
    case RED_BLACK_TREE:
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
    case SORTED_ARRAY:
      return SortedArray_PredecessorOp;
    case BINARY_TREE:
    case BINARY_TREE_UNBALANCED:
    case RED_BLACK_TREE:
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
    case SORTED_ARRAY:
      return SortedArray_SelectOp;
    case BINARY_TREE:
    case BINARY_TREE_UNBALANCED:
    case RED_BLACK_TREE:
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
    case SORTED_ARRAY:
      return SortedArray_RankOp;
    case BINARY_TREE:
    case BINARY_TREE_UNBALANCED:
    case RED_BLACK_TREE:
      return BinaryTree_RankOp;
    default:
      return NULL;
  }
}

double OperationTime(Operation op, Structure st, size_t n) {
  clock_t t;
  void* ds;

  switch (op) {
    case INSERT:
      // In the case of insert, n is ignored
      ds = BuildEmptyDataStructure(st);
      ListOp op = GetInsertOperation(st);
      if (ds == NULL || op == NULL) return -1;

      // Timed operation
      t = clock();
      for (size_t i = 0; i < n; i++) op(ds, rand());
      t = clock() - t;
      break;
    case SEARCH:
      ds = BuildDataStructure(st, n);
      SearchOp s_op = GetSearchOperation(st);
      if (ds == NULL || s_op == NULL) return -1;

      t = clock();
      for (size_t i = 0; i < n; i++) s_op(ds, rand());
      t = clock() - t;
      break;
    case ENUMERATE:
      ds = BuildDataStructure(st, n);
      EnumerateOp e_op = GetEnumerateOperation(st);
      _enumerate_sum = 0;

      t = clock();
      e_op(ds);
      t = clock() - t;
      break;
    case MAX:
      ds = BuildDataStructure(st, n);
      MaxOp m_op = GetMaxOperation(st);

      t = clock();
      for (size_t i = 0; i < n; i++) m_op(ds);
      t = clock() - t;
      break;
    case PREDECESSOR:
      ds = BuildDataStructure(st, n);
      PredOp p_op = GetPredOperation(st);

      t = clock();
      for (size_t i = 0; i < n; i++) p_op(ds, rand());
      t = clock() - t;
      break;
    case SELECT:
      ds = BuildDataStructure(st, n);
      SelectOp se_op = GetSelectOperation(st);

      t = clock();
      for (size_t i = 0; i < n; i++) se_op(ds, rand() % n);
      t = clock() - t;
      break;
    case RANK:
      ds = BuildDataStructure(st, n);
      RankOp r_op = GetRankOp(st);

      t = clock();
      for (size_t i = 0; i < n; i++) r_op(ds, rand());
      t = clock() - t;
      break;
  }

  if (ds == NULL) return -1;

  DestroyStructure(st, ds);
  double time = ((double)t) / CLOCKS_PER_SEC;
  return time;
}
