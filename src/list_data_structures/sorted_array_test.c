/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "./sorted_array.h"

#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "common_math.h"
#include "test_helpers.h"

typedef ResultCode (*SortedArrayOp)(const Array*, const void*, void**);
typedef ResultCode (*SortedArrayOpUnary)(const Array*, void**);
static unsigned int seed;

static Array* CreateSut(size_t n) {
  Array* sut = NULL;
  ResultCode result_code = Array_Create(PIntComparator, sizeof(int), &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  sut->array = calloc(sizeof(int), n);

  for (size_t i = 0; i < n; i++) ((int*)sut->array)[i] = rand_r(&seed);

  qsort(sut->array, n, sizeof(int), PIntComparator);
  sut->n = n;

  return sut;
}

static ResultCode SortedArray_Select_Unary(const Array* self, void** result) {
  return SortedArray_Select(self, 0, result);
}

static ResultCode SortedArray_Rank_ArrayOp(const Array* self, const void* item,
                                           void** dummy) {
  size_t result = 0;
  *dummy = NULL;
  return SortedArray_Rank(self, item, &result);
}

static void TestUnaryFuncNullParameter(SortedArrayOpUnary unary_funcs[]) {
  int tracker = 0;
  SortedArrayOpUnary uop = unary_funcs[0];
  while (uop != NULL) {
    void* result = NULL;
    ResultCode result_code = uop(NULL, &result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);
    CU_ASSERT_PTR_NULL(result);

    uop = unary_funcs[++tracker];
  }
}

static void TestFuncNullParameter(SortedArrayOp funcs[]) {
  void* item = malloc(100);
  Array* sut = CreateSut(10);

  int tracker = 0;
  SortedArrayOp op = funcs[0];
  while (op != NULL) {
    void* result = NULL;
    ResultCode result_code = op(NULL, NULL, &result);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code = op(NULL, item, &result);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code = op(sut, NULL, &result);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    op = funcs[++tracker];
  }

  SortedArrayOpUnary unary_funcs[] = {SortedArray_Min, SortedArray_Max,
                                      SortedArray_Select_Unary, NULL};
  TestUnaryFuncNullParameter(unary_funcs);

  free(item);
  Array_Destroy(sut);
}

static void SortedArray_null_parameter() {
  SortedArrayOp funcs[] = {SortedArray_Search, SortedArray_Successor,
                           SortedArray_Predecessor, SortedArray_Rank_ArrayOp,
                           NULL};
  TestFuncNullParameter(funcs);

  SortedArrayOpUnary unary_funcs[] = {SortedArray_Min, SortedArray_Max,
                                      SortedArray_Select_Unary, NULL};
  TestUnaryFuncNullParameter(unary_funcs);
}

static void SortedArray_Search_not_found() {
  Array* sut = CreateSut(10);
  // if the first item is 0, this could wrap but the risk is minmal
  int not_there = ((int*)sut->array)[0] - 1;

  void* result = NULL;
  ResultCode result_code = SortedArray_Search(sut, &not_there, &result);
  CU_ASSERT_EQUAL(result_code, kNotFound);
  CU_ASSERT_PTR_NULL(result);

  Array_Destroy(sut);
}

static void SortedArray_Search_standard() {
  const size_t n = 15;

  Array* sut = CreateSut(n);

  for (size_t i = 0; i < n; i++) {
    int there = ((int*)sut->array)[i];
    void* result = NULL;
    ResultCode result_code = SortedArray_Search(sut, &there, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_PTR_EQUAL(result, (char*)sut->array + sizeof(int) * i);
  }

  Array_Destroy(sut);
}

static void SortedArray_Min_empty() {
  Array* sut = CreateSut(0);

  void* result = NULL;
  ResultCode result_code = SortedArray_Min(sut, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kEmpty);

  Array_Destroy(sut);
}

static void SortedArray_Min_standard() {
  Array* sut = CreateSut(10);

  void* result = NULL;
  ResultCode result_code = SortedArray_Min(sut, &result);
  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(sut->array, result);

  Array_Destroy(sut);
}

static void SortedArray_Max_empty() {
  Array* sut = CreateSut(0);

  void* result = NULL;
  ResultCode result_code = SortedArray_Max(sut, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kEmpty);

  Array_Destroy(sut);
}

static void SortedArray_Max_standard() {
  Array* sut = CreateSut(10);

  void* result = NULL;
  ResultCode result_code = SortedArray_Max(sut, &result);
  CU_ASSERT_EQUAL(result_code, kSuccess);
  int* arr = sut->array;

  CU_ASSERT_EQUAL(&arr[9], result);

  Array_Destroy(sut);
}

static void SortedArray_Predecessor_not_found() {
  Array* sut = CreateSut(10);
  int not_found = *(int*)sut->array - 1;

  void* result = NULL;
  ResultCode result_code = SortedArray_Predecessor(sut, &not_found, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kNotFound);

  Array_Destroy(sut);
}

static void SortedArray_Predecessor_empty() {
  Array* sut = CreateSut(0);
  int item = 10;

  void* result = NULL;
  ResultCode result_code = SortedArray_Predecessor(sut, &item, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kEmpty);

  Array_Destroy(sut);
}

static void SortedArray_Predecessor_standard() {
  const size_t n = 10;
  Array* sut = CreateSut(n);
  int* array_t = (int*)sut->array;

  for (size_t i = 1; i < n; i++) {
    int search_for = array_t[i];

    void* result = NULL;
    ResultCode result_code = SortedArray_Predecessor(sut, &search_for, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_PTR_EQUAL(&array_t[i - 1], result);
  }

  Array_Destroy(sut);
}

static void SortedArray_Predecessor_first_item() {
  Array* sut = CreateSut(10);

  int search_for = *(int*)sut->array;

  void* result = NULL;
  ResultCode result_code = SortedArray_Predecessor(sut, &search_for, &result);
  CU_ASSERT_EQUAL(result_code, kInvalidIndex);
  CU_ASSERT_PTR_NULL(result);

  Array_Destroy(sut);
}

static void SortedArray_Successor_not_found() {
  Array* sut = CreateSut(10);
  int not_found = *(int*)sut->array - 1;

  void* result = NULL;
  ResultCode result_code = SortedArray_Successor(sut, &not_found, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kNotFound);

  Array_Destroy(sut);
}

static void SortedArray_Successor_empty() {
  Array* sut = CreateSut(0);
  int item = 10;

  void* result = NULL;
  ResultCode result_code = SortedArray_Successor(sut, &item, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kEmpty);

  Array_Destroy(sut);
}

static void SortedArray_Successor_standard() {
  const size_t n = 10;
  Array* sut = CreateSut(n);
  int* array_t = (int*)sut->array;

  for (size_t i = 0; i < n - 1; i++) {
    int search_for = array_t[i];

    void* result = NULL;
    ResultCode result_code = SortedArray_Successor(sut, &search_for, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_PTR_EQUAL(&array_t[i + 1], result);
  }

  Array_Destroy(sut);
}

static void SortedArray_Successor_last_item() {
  const size_t n = 10;
  Array* sut = CreateSut(n);

  int search_for = ((int*)sut->array)[n - 1];

  void* result = NULL;
  ResultCode result_code = SortedArray_Successor(sut, &search_for, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kInvalidIndex);

  Array_Destroy(sut);
}

static void SortedArray_Select_out_of_bounds() {
  const size_t n = 10;
  Array* sut = CreateSut(n);

  void* result = NULL;
  ResultCode result_code = SortedArray_Select(sut, n, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kInvalidIndex);
  Array_Destroy(sut);
}

static void SortedArray_Select_standard() {
  const size_t n = 10;
  Array* sut = CreateSut(n);
  int* array_t = (int*)sut->array;

  for (size_t i = 0; i < n; i++) {
    void* result = NULL;
    ResultCode result_code = SortedArray_Select(sut, i, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_PTR_EQUAL(&array_t[i], result);
  }

  Array_Destroy(sut);
}

static void SortedArray_Rank_not_found() {
  Array* sut = CreateSut(10);
  int not_found = *(int*)sut->array - 1;

  size_t result = 0;
  ResultCode result_code = SortedArray_Rank(sut, &not_found, &result);
  CU_ASSERT_EQUAL(result_code, kNotFound);

  Array_Destroy(sut);
}

static void SortedArray_Rank_empty() {
  Array* sut = CreateSut(0);
  int item = 10;

  size_t result = 0;
  ResultCode result_code = SortedArray_Rank(sut, &item, &result);
  CU_ASSERT_EQUAL(result_code, kEmpty);

  Array_Destroy(sut);
}

static void SortedArray_Rank_standard() {
  const size_t n = 10;
  Array* sut = CreateSut(n);
  int* array_t = (int*)sut->array;

  for (size_t i = 0; i < n; i++) {
    int search_for = array_t[i];
    size_t result = 0;
    ResultCode result_code = SortedArray_Rank(sut, &search_for, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_PTR_EQUAL(i, result);
  }

  Array_Destroy(sut);
}

int RegisterSortedArrayTests() {
  seed = time(NULL);

  CU_TestInfo Common_tests[] = {CU_TEST_INFO(SortedArray_null_parameter),
                                CU_TEST_INFO_NULL};

  CU_TestInfo Search_tests[] = {CU_TEST_INFO(SortedArray_Search_not_found),
                                CU_TEST_INFO(SortedArray_Search_standard),
                                CU_TEST_INFO_NULL};

  CU_TestInfo Min_tests[] = {CU_TEST_INFO(SortedArray_Min_empty),
                             CU_TEST_INFO(SortedArray_Min_standard),
                             CU_TEST_INFO_NULL};

  CU_TestInfo Max_tests[] = {CU_TEST_INFO(SortedArray_Max_empty),
                             CU_TEST_INFO(SortedArray_Max_standard),
                             CU_TEST_INFO_NULL};

  CU_TestInfo Predecessor_tests[] = {
      CU_TEST_INFO(SortedArray_Predecessor_not_found),
      CU_TEST_INFO(SortedArray_Predecessor_empty),
      CU_TEST_INFO(SortedArray_Predecessor_standard),
      CU_TEST_INFO(SortedArray_Predecessor_first_item), CU_TEST_INFO_NULL};

  CU_TestInfo Successor_tests[] = {
      CU_TEST_INFO(SortedArray_Successor_not_found),
      CU_TEST_INFO(SortedArray_Successor_empty),
      CU_TEST_INFO(SortedArray_Successor_standard),
      CU_TEST_INFO(SortedArray_Successor_last_item), CU_TEST_INFO_NULL};

  CU_TestInfo Select_tests[] = {CU_TEST_INFO(SortedArray_Select_out_of_bounds),
                                CU_TEST_INFO(SortedArray_Select_standard),
                                CU_TEST_INFO_NULL};

  CU_TestInfo Rank_tests[] = {CU_TEST_INFO(SortedArray_Rank_not_found),
                              CU_TEST_INFO(SortedArray_Rank_empty),
                              CU_TEST_INFO(SortedArray_Rank_standard),
                              CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "SortedArray Common",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Common_tests},
                           {.pName = "SortedArray_Search",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Search_tests},
                           {.pName = "SortedArray_Min",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Min_tests},
                           {.pName = "SortedArray_Max",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Max_tests},
                           {.pName = "SortedArray_Predecessor",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Predecessor_tests},
                           {.pName = "SortedArray_Successor",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Successor_tests},
                           {.pName = "SortedArray_Select",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Select_tests},
                           {.pName = "SortedArray_Rank",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Rank_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
