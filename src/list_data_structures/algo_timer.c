/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "algo_timer.h"

#include <stdlib.h>
#include <time.h>

#include "result_code.h"

typedef ResultCode (*InsertOp)(void*, uintptr_t);
typedef ResultCode (*SearchOp)(const void*, const uintptr_t);
typedef ResultCode (*EnumerateOp)(const void*);
typedef ResultCode (*MaxOp)(const void*);
typedef ResultCode (*PredOp)(const void*, const uintptr_t);
typedef ResultCode (*RankOp)(const void*, const uintptr_t);

static InsertOp GetInsertOperation(Structure str);
static uintptr_t _enumerate_sum = 0;

static int PointerAsIntComparator(const void* x, const void* y) {
  if (x == y) return 0;

  uintptr_t _x = (uintptr_t)x;
  uintptr_t _y = (uintptr_t)y;

  if (_x == _y) return 0;
  if (_x < _y) return -1;
  return 1;
}

static ResultCode BuildEmptyDataStructure(Structure str, void** result) {
  switch (str) {
    case kArray:
    case kSortedArray:
      return Array_Create(PointerAsIntComparator, sizeof(uintptr_t),
                          (Array**)result);
    case kLinkedList:
    case kLinkedListPoorLocality:
      return LinkedList_Create(NULL, PointerAsIntComparator,
                               (LinkedList**)result);
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      return BinaryTree_Create(PointerAsIntComparator, (BinaryTree**)result);
  }

  return kNotFound;
}

static ResultCode BuildDataStructure(Structure str, size_t n, void** result) {
  unsigned int seed = time(NULL);
  void* ds = NULL;

  ResultCode result_code = BuildEmptyDataStructure(str, &ds);
  if (result_code != kSuccess) return result_code;

  InsertOp op = GetInsertOperation(str);
  if (ds == NULL) return kNotFound;

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
      return kNotFound;
  }

  *result = ds;
  return kSuccess;
}

static void DestroyStructure(Structure str, void* structure) {
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

/*******************************************************************************
 * Insert Operations
 ******************************************************************************/
static ResultCode Array_InsertOp(void* array, uintptr_t item) {
  return Array_InsertAtHead(array, &item);
}

static ResultCode Array_InsertAtTailOp(void* array, uintptr_t item) {
  return Array_InsertAtTail(array, &item);
}

static ResultCode LinkedList_InsertOp(void* list, uintptr_t item) {
  return LinkedList_InsertAt(list, (void*)item, 0);
}

static ResultCode LinkedList_InsertAtTailOp(void* list, uintptr_t item) {
  LinkedList* list_t = (LinkedList*)list;
  return LinkedList_InsertAt(list, (void*)item, list_t->size);
}

static ResultCode BinaryTree_InsertOp(void* tree, uintptr_t item) {
  return BinaryTree_Insert(tree, (void*)item);
}

static ResultCode RedBlackTree_InsertOp(void* tree, uintptr_t item) {
  return RedBlackTree_Insert(tree, (void*)item);
}

static InsertOp GetInsertOperation(Structure str) {
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
    default:
      return NULL;
  }
}

static InsertOp GetInsertAtTailOperation(Structure str) {
  switch (str) {
    case kArray:
    case kSortedArray:
      return Array_InsertAtTailOp;
    case kLinkedList:
    case kLinkedListPoorLocality:
      return LinkedList_InsertAtTailOp;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
      return BinaryTree_InsertOp;
    case kRedBlackTree:
      return RedBlackTree_InsertOp;
    default:
      return NULL;
  }
}

/*******************************************************************************
 * Search Operations
 ******************************************************************************/
static ResultCode Array_SearchOp(const void* array, const uintptr_t item) {
  void* result = NULL;
  return Array_Search(array, &item, &result);
}

static ResultCode SortedArray_SearchOp(const void* array,
                                       const uintptr_t item) {
  void* result = NULL;
  return SortedArray_Search(array, &item, &result);
}

static ResultCode LinkedList_SearchOp(const void* list, const uintptr_t item) {
  void* result = NULL;
  return LinkedList_Search(list, (void*)item, &result);
}

static ResultCode BinaryTree_SearchOp(const void* tree, const uintptr_t item) {
  void* result = NULL;
  return BinaryTree_Search(tree, (void*)item, &result);
}

static SearchOp GetSearchOperation(Structure str) {
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

/*******************************************************************************
 * Enumerate Operations
 ******************************************************************************/
static void Array_EnumerateHandler(void* item) {
  _enumerate_sum += *(uintptr_t*)item;
}

static ResultCode Array_EnumerateOp(const void* array) {
  return Array_Enumerate(array, Array_EnumerateHandler);
}

static void ListAndTree_EnumerateHandler(void* item) {
  _enumerate_sum += (uintptr_t)item;
}

static ResultCode LinkedList_EnumerateOp(const void* list) {
  return LinkedList_Enumerate(list, ListAndTree_EnumerateHandler);
}

static ResultCode BinaryTree_EnumerateOp(const void* tree) {
  return BinaryTree_Enumerate(tree, ListAndTree_EnumerateHandler);
}

static EnumerateOp GetEnumerateOperation(Structure str) {
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

/*******************************************************************************
 * Max Operations
 ******************************************************************************/
static ResultCode Array_MaxOp(const void* array) {
  void* result = NULL;
  return Array_Max(array, &result);
}

static ResultCode SortedArray_MaxOp(const void* array) {
  void* result = NULL;
  return SortedArray_Max(array, &result);
}

static ResultCode LinkedList_MaxOp(const void* linked_list) {
  void* result = NULL;
  return LinkedList_Max(linked_list, &result);
}

static ResultCode BinaryTree_MaxOp(const void* tree) {
  void* result = NULL;
  return BinaryTree_Max(tree, &result);
}

MaxOp GetMaxOperation(Structure str) {
  switch (str) {
    case kArray:
      return Array_MaxOp;
    case kSortedArray:
      return SortedArray_MaxOp;
    case kLinkedList:
    case kLinkedListPoorLocality:
      return LinkedList_MaxOp;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      return BinaryTree_MaxOp;
    default:
      return NULL;
  }
}

/*******************************************************************************
 * Predecessor Operations
 ******************************************************************************/
static ResultCode Array_PredecessorOp(const void* array, const uintptr_t item) {
  void* result = NULL;
  return Array_Predecessor(array, &item, &result);
}

static ResultCode LinkedList_PredecessorOp(const void* linked_list,
                                           const uintptr_t item) {
  void* result = NULL;
  return LinkedList_Predecessor(linked_list, &item, &result);
}

static ResultCode SortedArray_PredecessorOp(const void* array,
                                            const uintptr_t item) {
  void* result = NULL;
  return SortedArray_Predecessor(array, &item, &result);
}

static ResultCode BinaryTree_PredecessorOp(const void* tree,
                                           const uintptr_t item) {
  void* result = NULL;
  return BinaryTree_Predecessor(tree, &item, &result);
}

static PredOp GetPredOperation(Structure str) {
  switch (str) {
    case kArray:
      return Array_PredecessorOp;
    case kSortedArray:
      return SortedArray_PredecessorOp;
    case kLinkedList:
    case kLinkedListPoorLocality:
      return LinkedList_PredecessorOp;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      return BinaryTree_PredecessorOp;
    default:
      return NULL;
  }
}

/*******************************************************************************
 * Rank Operations
 ******************************************************************************/
ResultCode Array_RankOp(const void* array, const size_t item) {
  size_t result;
  return Array_Rank(array, &item, &result);
}

ResultCode LinkedList_RankOp(const void* linked_list, const size_t item) {
  size_t result;
  return LinkedList_Rank(linked_list, &item, &result);
}

ResultCode SortedArray_RankOp(const void* array, const size_t item) {
  size_t result;
  return SortedArray_Rank(array, &item, &result);
}

ResultCode BinaryTree_RankOp(const void* tree, const size_t item) {
  size_t result;
  return BinaryTree_Rank(tree, &item, &result);
}

RankOp GetRankOp(Structure str) {
  switch (str) {
    case kArray:
      return Array_RankOp;
    case kSortedArray:
      return SortedArray_RankOp;
    case kLinkedList:
    case kLinkedListPoorLocality:
      return LinkedList_RankOp;
    case kBinaryTree:
    case kBinaryTreeUnbalanced:
    case kRedBlackTree:
      return BinaryTree_RankOp;
    default:
      return NULL;
  }
}

double ListDataStructures_OperationTime(Operation op, Structure st, size_t n) {
  unsigned int seed = time(NULL);
  clock_t t;
  ResultCode result_code;
  void* ds = NULL;

  switch (op) {
    case kInsertAtTail:
    case kInsert:
      // In the case of insert, n is ignored
      result_code = BuildEmptyDataStructure(st, &ds);
      if (result_code != kSuccess) {
        PRINT_ERROR("BuildEmptyDataStructure", result_code);
        return -1;
      }

      InsertOp i_op = (op == kInsertAtTail) ? GetInsertAtTailOperation(st)
                                            : GetInsertOperation(st);
      if (i_op == NULL) {
        PRINT_ERROR("GetInsertOperation", kNotFound);
        return -1;
      }

      // Timed operation
      t = clock();
      // No error checking to avoid time overhead
      if (st == kBinaryTreeUnbalanced) {
        // Insert in sequential order so tree is completely unbalanced
        for (size_t i = 0; i < n; i++) i_op(ds, i);
      } else {
        for (size_t i = 0; i < n; i++) i_op(ds, rand_r(&seed));
      }
      t = clock() - t;
      break;
    case kSearch:
      result_code = BuildDataStructure(st, n, &ds);
      if (result_code != kSuccess) {
        PRINT_ERROR("BuildDataStructure", result_code);
        return -1;
      }

      SearchOp s_op = GetSearchOperation(st);
      if (s_op == NULL) {
        PRINT_ERROR("GetSearchOperation", kNotFound);
        return -1;
      }

      t = clock();
      // No error checking to avoid time overhead
      for (size_t i = 0; i < n; i++) s_op(ds, rand_r(&seed));
      t = clock() - t;
      break;
    case kEnumerate:
      result_code = BuildDataStructure(st, n, &ds);
      if (result_code != kSuccess) {
        PRINT_ERROR("BuildDataStructure", result_code);
        return -1;
      }

      EnumerateOp e_op = GetEnumerateOperation(st);
      if (e_op == NULL) {
        PRINT_ERROR("GetEnumerateOperation", kNotFound);
        return -1;
      }
      _enumerate_sum = 0;

      t = clock();
      // No error checking to avoid time overhead
      e_op(ds);
      t = clock() - t;
      break;
    case kMax:
      result_code = BuildDataStructure(st, n, &ds);
      if (result_code != kSuccess) {
        PRINT_ERROR("BuildDataStructure", result_code);
        return -1;
      }

      MaxOp m_op = GetMaxOperation(st);
      if (m_op == NULL) {
        PRINT_ERROR("GetMaxOperation", kNotFound);
        return -1;
      }

      t = clock();
      for (size_t i = 0; i < n; i++) m_op(ds);
      t = clock() - t;
      break;
    case kPredecessor:
      result_code = BuildDataStructure(st, n, &ds);
      if (result_code != kSuccess) {
        PRINT_ERROR("BuildDataStructure", result_code);
        return -1;
      }

      PredOp p_op = GetPredOperation(st);
      if (p_op == NULL) {
        PRINT_ERROR("GetPredOperation", kNotFound);
        return -1;
      }

      t = clock();
      // No error checking to avoid time overhead
      for (size_t i = 0; i < n; i++) p_op(ds, rand_r(&seed));
      t = clock() - t;
      break;
    case kRank:
      result_code = BuildDataStructure(st, n, &ds);
      if (result_code != kSuccess) {
        PRINT_ERROR("BuildDataStructure", result_code);
        return -1;
      }

      RankOp r_op = GetRankOp(st);
      if (r_op == NULL) {
        PRINT_ERROR("GetRankOperation", kNotFound);
        return -1;
      }

      t = clock();
      // No error checking to avoid time overhead
      for (size_t i = 0; i < n; i++) r_op(ds, rand_r(&seed));
      t = clock() - t;
      break;
  }

  if (ds == NULL) return -1;

  DestroyStructure(st, ds);
  double time = ((double)t) / CLOCKS_PER_SEC;
  return time;
}
