/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "array.h"

#include <CUnit/CUnit.h>
#include <stdbool.h>
#include <stdlib.h>
#if !defined(NDEBUG)
#include "malloc_test_wrapper.h"
#endif
#include "test_helpers.h"

#define INT2VOIDP(i) (void*)(uintptr_t)(i)
#define VOIDP2INT(i) *(int*)(i)
#define SUT(vals, n, code_block)                                              \
  {                                                                           \
    Array* sut = NULL;                                                        \
    ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &sut); \
    CU_ASSERT_EQUAL(result_code, kSuccess);                                   \
    for (size_t i = 0; i < (n); i++) {                                        \
      result_code = Array_InsertAtTail(sut, &(vals)[i]);                      \
      CU_ASSERT_EQUAL(result_code, kSuccess);                                 \
    }                                                                         \
    {code_block};                                                             \
    Array_Destroy(sut);                                                       \
  }

/*************************** Array_Create *************************************/
static void Array_Create_bad_malloc() {
#if !defined(NDEBUG)
  FAILED_MALLOC_TEST({
    Array* array = NULL;
    ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
    CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
    CU_ASSERT_PTR_NULL(array);
  });
#endif
}

static void Array_Create_inits_values() {
  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  CU_ASSERT_PTR_EQUAL(PIntComparator, array->comparator);
  CU_ASSERT_EQUAL(sizeof(int), array->item_size);
  CU_ASSERT_EQUAL(0, array->n);
  CU_ASSERT_PTR_NULL(array->array);

  Array_Destroy(array);
}

/*************************** Array_InsertAtHead *******************************/
static void Array_InsertAtHead_null_paramter() {
  int dummy = 5;
  ResultCode result_code = Array_InsertAtHead(NULL, &dummy);

  CU_ASSERT_EQUAL(result_code, kNullParameter);

  Array* array = NULL;
  result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  result_code = Array_InsertAtHead(array, NULL);
  CU_ASSERT_EQUAL(result_code, kNullParameter);
  CU_ASSERT_EQUAL(0, array->n);

  Array_Destroy(array);
}

static void Array_InsertAtHead_bad_malloc() {
#if !defined(NDEBUG)
  int first = 1;

  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  FAILED_MALLOC_TEST({
    result_code = Array_InsertAtHead(array, &first);

    CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
    CU_ASSERT_EQUAL(0, array->n);
  });

  Array_Destroy(array);
#endif
}

static void Array_InsertAtHead_first_item() {
  int first = 1;

  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  result_code = Array_InsertAtHead(array, &first);

  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(1, array->n);
  int* a = (int*)array->array;
  CU_ASSERT_EQUAL(first, a[0]);

  Array_Destroy(array);
}

static void Array_InsertAtHead_standard() {
  const size_t n = 5;
  const int items[] = {1, 2, 3, 4, 5};
  const int expected[] = {5, 4, 3, 2, 1};

  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  for (size_t i = 0; i < n; i++) {
    result_code = Array_InsertAtHead(array, &items[i]);
    CU_ASSERT_EQUAL(result_code, kSuccess);
  }

  CU_ASSERT_EQUAL(n, array->n);
  CU_ASSERT_EQUAL(0, memcmp(expected, array->array, sizeof(int) * n));

  Array_Destroy(array);
}

static void Array_InsertAtHead_bad_malloc_on_realloc() {
#if !defined(NDEBUG)
  int first = 1;
  int second = 2;

  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  result_code = Array_InsertAtHead(array, &first);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  FAILED_MALLOC_TEST({
    result_code = Array_InsertAtHead(array, &second);
    CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
    CU_ASSERT_EQUAL(1, array->n);
  });

  Array_Destroy(array);
#endif
}

/*************************** Array_InsertAtTail *******************************/
static void Array_InsertAtTail_null_paramter() {
  int dummy = 5;
  ResultCode result_code = Array_InsertAtTail(NULL, &dummy);

  CU_ASSERT_EQUAL(result_code, kNullParameter);

  Array* array = NULL;
  result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  result_code = Array_InsertAtTail(array, NULL);
  CU_ASSERT_EQUAL(result_code, kNullParameter);
  CU_ASSERT_EQUAL(0, array->n);

  Array_Destroy(array);
}

static void Array_InsertAtTail_bad_malloc() {
#if !defined(NDEBUG)
  int first = 1;

  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  FAILED_MALLOC_TEST({
    result_code = Array_InsertAtTail(array, &first);

    CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
    CU_ASSERT_EQUAL(0, array->n);
  });

  Array_Destroy(array);
#endif
}

static void Array_InsertAtTail_first_item() {
  int first = 1;

  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  result_code = Array_InsertAtTail(array, &first);

  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(1, array->n);
  int* a = (int*)array->array;
  CU_ASSERT_EQUAL(first, a[0]);

  Array_Destroy(array);
}

static void Array_InsertAtTail_standard() {
  const size_t n = 5;
  const int items[] = {1, 2, 3, 4, 5};
  const int expected[] = {1, 2, 3, 4, 5};

  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  for (size_t i = 0; i < n; i++) {
    result_code = Array_InsertAtTail(array, &items[i]);
    CU_ASSERT_EQUAL(result_code, kSuccess);
  }

  CU_ASSERT_EQUAL(n, array->n);
  CU_ASSERT_EQUAL(0, memcmp(expected, array->array, sizeof(int) * n));

  Array_Destroy(array);
}

static void Array_InsertAtTail_bad_malloc_on_realloc() {
#if !defined(NDEBUG)
  int first = 1;
  int second = 2;

  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  result_code = Array_InsertAtTail(array, &first);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  FAILED_MALLOC_TEST({
    result_code = Array_InsertAtTail(array, &second);
    CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
    CU_ASSERT_EQUAL(1, array->n);
  });

  Array_Destroy(array);
#endif
}

/*************************** Array_Search *************************************/
static void Array_Search_null_parameter() {
  ResultCode result_code = Array_Search(NULL, NULL, NULL);
  CU_ASSERT_EQUAL(result_code, kNullParameter);

  void* result = NULL;
  Array* array = NULL;
  result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  result_code = Array_Search(array, NULL, result);
  CU_ASSERT_EQUAL(result_code, kNullParameter);

  int dummy = 5;
  result_code = Array_Search(NULL, &dummy, result);
  CU_ASSERT_EQUAL(result_code, kNullParameter);

  Array_Destroy(array);
}

static void Array_Search_not_found() {
  const size_t n = 5;
  const int dummy = 10;
  const int items[] = {1, 2, 3, 4, 5};

  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  for (size_t i = 0; i < n; i++) {
    result_code = Array_InsertAtHead(array, &items[i]);
    CU_ASSERT_EQUAL(kSuccess, result_code);
  }

  void* result = NULL;
  result_code = Array_Search(array, &dummy, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kNotFound);

  Array_Destroy(array);
}

static void Array_Search_standard() {
  const size_t n = 5;
  const int find = 2;
  const int find2 = 4;
  const int items[] = {1, 2, 3, 4, 5};

  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  for (size_t i = 0; i < n; i++) {
    result_code = Array_InsertAtHead(array, &items[i]);
    CU_ASSERT_EQUAL(result_code, kSuccess);
  }

  void* result = NULL;
  result_code = Array_Search(array, &find, &result);
  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_PTR_EQUAL(result, (char*)array->array + (array->item_size * 3));

  result = NULL;
  result_code = Array_Search(array, &find2, &result);
  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_PTR_EQUAL(result, (char*)array->array + array->item_size);

  Array_Destroy(array);
}

/*************************** Array_Enumerate **********************************/
static const size_t mock_n = 13;
static int mock_vals[] = {50, 25, 33, 11, 30, 40, 75, 61, 89, 52, 82, 95, 55};
static int mock_ordered[] = {11, 25, 30, 33, 40, 50, 52,
                             55, 61, 75, 82, 89, 95};
static int mock_pos = 0;
static void MockItemHandler(void* item) {
  // The items are inserted into the head, so they are backwards from how they
  // are inserted
  CU_ASSERT_EQUAL(mock_vals[mock_n - 1 - mock_pos], VOIDP2INT(item));
  mock_pos++;
}

static void Array_Enumerate_null_parameter() {
  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  result_code = Array_Enumerate(array, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Array_Enumerate(NULL, MockItemHandler);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  Array_Destroy(array);
}

static void Array_Enumerate_empty() {
  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  result_code = Array_Enumerate(array, MockItemHandler);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  Array_Destroy(array);
}

static void Array_Enumerate_standard() {
  Array* array = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &array);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  for (size_t i = 0; i < mock_n; i++) Array_InsertAtHead(array, &mock_vals[i]);

  mock_pos = 0;
  result_code = Array_Enumerate(array, MockItemHandler);
  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(mock_n, mock_pos);

  Array_Destroy(array);
}

/*************************** Array_Max ****************************************/
static void Array_Max_empty() {
  Array* sut = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  void* result = NULL;
  result_code = Array_Max(sut, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kEmpty);

  Array_Destroy(sut);
}

static void Array_Max_standard() {
  Array* sut = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  for (size_t i = 0; i < mock_n; i++) Array_InsertAtHead(sut, &mock_vals[i]);

  void* result = NULL;
  result_code = Array_Max(sut, &result);
  CU_ASSERT_EQUAL(kSuccess, result_code);
  CU_ASSERT_EQUAL(95, *(int*)result);

  Array_Destroy(sut);
}

/*************************** Array_Predecessor ********************************/
static void Array_Predecessor_empty() {
  Array* sut = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);
  int search_for = 5;

  void* result = NULL;
  result_code = Array_Predecessor(sut, &search_for, &result);

  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kEmpty);
  Array_Destroy(sut);
}

static void Array_Predecessor_not_found() {
  SUT(mock_vals, mock_n, {
    int not_found = 401;

    void* result = NULL;
    ResultCode result_code = Array_Predecessor(sut, &not_found, &result);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kNotFound);
  });
}

static void Array_Predecessor_first_item() {
  SUT(mock_vals, mock_n, {
    int first = 11;

    void* result = NULL;
    ResultCode result_code = Array_Predecessor(sut, &first, &result);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
  });
}

static void Array_Predecessor_standard() {
  SUT(mock_vals, mock_n, {
    for (size_t i = 1; i < mock_n; i++) {
      void* result = NULL;
      result_code = Array_Predecessor(sut, &mock_ordered[i], &result);
      CU_ASSERT_EQUAL(result_code, kSuccess);
      CU_ASSERT_EQUAL(mock_ordered[i - 1], *(int*)result);
    }
  });
}

/*************************** Array_Rank ***************************************/
static void Array_Rank_null_paramter() {
  SUT(mock_vals, mock_n, {
    int search_for = 5;

    size_t result = 0;

    ResultCode result_code = Array_Rank(NULL, NULL, &result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code = Array_Rank(sut, NULL, &result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code = Array_Rank(sut, &search_for, NULL);
    CU_ASSERT_EQUAL(result_code, kNullParameter);
  });
}

static void Array_Rank_empty() {
  Array* sut = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  int search_for = 5;

  size_t result = 0;
  result_code = Array_Rank(sut, &search_for, &result);
  CU_ASSERT_EQUAL(result_code, kEmpty);

  Array_Destroy(sut);
}

static void Array_Rank_not_found() {
  SUT(mock_vals, mock_n, {
    int not_found = 401;
    size_t result = 0;
    ResultCode result_code = Array_Rank(sut, &not_found, &result);
    CU_ASSERT_EQUAL(result_code, kNotFound);
  });
}

static void Array_Rank_standard() {
  SUT(mock_vals, mock_n, {
    for (size_t i = 0; i < mock_n; i++) {
      size_t result = 0;
      ResultCode result_code = Array_Rank(sut, &mock_ordered[i], &result);
      CU_ASSERT_EQUAL(result, i);
      CU_ASSERT_EQUAL(result_code, kSuccess);
    }
  });
}

/*************************** Array_Destory ************************************/
static void Array_Destroy_null_parameter() {
  // if no error, this is cool
  Array_Destroy(NULL);
}

int RegisterArrayTests() {
  CU_TestInfo Create_tests[] = {CU_TEST_INFO(Array_Create_bad_malloc),
                                CU_TEST_INFO(Array_Create_inits_values),
                                CU_TEST_INFO_NULL};

  CU_TestInfo InsertAtHead_tests[] = {
      CU_TEST_INFO(Array_InsertAtHead_null_paramter),
      CU_TEST_INFO(Array_InsertAtHead_bad_malloc),
      CU_TEST_INFO(Array_InsertAtHead_first_item),
      CU_TEST_INFO(Array_InsertAtHead_standard),
      CU_TEST_INFO(Array_InsertAtHead_bad_malloc_on_realloc),
      CU_TEST_INFO_NULL};

  CU_TestInfo InsertAtTail_tests[] = {
      CU_TEST_INFO(Array_InsertAtTail_null_paramter),
      CU_TEST_INFO(Array_InsertAtTail_bad_malloc),
      CU_TEST_INFO(Array_InsertAtTail_first_item),
      CU_TEST_INFO(Array_InsertAtTail_standard),
      CU_TEST_INFO(Array_InsertAtTail_bad_malloc_on_realloc),
      CU_TEST_INFO_NULL};

  CU_TestInfo Search_tests[] = {CU_TEST_INFO(Array_Search_null_parameter),
                                CU_TEST_INFO(Array_Search_not_found),
                                CU_TEST_INFO(Array_Search_standard),
                                CU_TEST_INFO_NULL};

  CU_TestInfo Enumerate_tests[] = {CU_TEST_INFO(Array_Enumerate_null_parameter),
                                   CU_TEST_INFO(Array_Enumerate_empty),
                                   CU_TEST_INFO(Array_Enumerate_standard),
                                   CU_TEST_INFO_NULL};

  CU_TestInfo Max_tests[] = {CU_TEST_INFO(Array_Max_empty),
                             CU_TEST_INFO(Array_Max_standard),
                             CU_TEST_INFO_NULL};

  CU_TestInfo Predecessor_tests[] = {CU_TEST_INFO(Array_Predecessor_empty),
                                     CU_TEST_INFO(Array_Predecessor_not_found),
                                     CU_TEST_INFO(Array_Predecessor_first_item),
                                     CU_TEST_INFO(Array_Predecessor_standard),
                                     CU_TEST_INFO_NULL};

  CU_TestInfo Rank_tests[] = {
      CU_TEST_INFO(Array_Rank_null_paramter), CU_TEST_INFO(Array_Rank_empty),
      CU_TEST_INFO(Array_Rank_not_found), CU_TEST_INFO(Array_Rank_standard),
      CU_TEST_INFO_NULL};

  CU_TestInfo Destroy_tests[] = {CU_TEST_INFO(Array_Destroy_null_parameter),
                                 CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Array_Create",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Create_tests},
                           {.pName = "Array_InsertAtHead",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = InsertAtHead_tests},
                           {.pName = "Array_InsertAtTail",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = InsertAtTail_tests},
                           {.pName = "Array_Search",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Search_tests},
                           {.pName = "Array_Enumerate",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Enumerate_tests},
                           {.pName = "Array_Max",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Max_tests},
                           {.pName = "Array_Predecessor",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Predecessor_tests},
                           {.pName = "Array_Rank",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Rank_tests},
                           {.pName = "Array_Destroy",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Destroy_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
