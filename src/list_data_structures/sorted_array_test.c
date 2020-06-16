// Copyright 2020 Dale Alleshouse
#include "./sorted_array.h"

#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "../utils/error_reporter.h"
#include "../utils/math.h"
#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

typedef void* (*SortedArrayOp)(const Array*, const void*);
typedef void* (*SortedArrayOpUnary)(const Array*);
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

static void* SortedArray_Select_Unary(const Array* self) {
  return SortedArray_Select(self, 0);
}

static void* SortedArray_Rank_ArrayOp(const Array* self, const void* item) {
  size_t result = SortedArray_Rank(self, item);

  CU_ASSERT_EQUAL(RANK_ERROR, result);
  return NULL;
}

static void TestUnaryFunckNullParameter(SortedArrayOpUnary unary_funcs[]) {
  int tracker = 0;
  SortedArrayOpUnary uop = unary_funcs[0];
  while (uop != NULL) {
    ErrorReporter_Clear();
    void* result = uop(NULL);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(kkNullParameter, ErrorReporter_LastErrorCode());

    uop = unary_funcs[++tracker];
  }
}

static void TestFunckNullParameter(SortedArrayOp funcs[]) {
  void* item = malloc(100);
  Array* sut = CreateSut(10);

  int tracker = 0;
  SortedArrayOp op = funcs[0];
  while (op != NULL) {
    ErrorReporter_Clear();
    void* result = op(NULL, NULL);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(kkNullParameter, ErrorReporter_LastErrorCode());

    ErrorReporter_Clear();
    result = op(NULL, item);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(kkNullParameter, ErrorReporter_LastErrorCode());

    ErrorReporter_Clear();
    result = op(sut, NULL);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(kkNullParameter, ErrorReporter_LastErrorCode());

    op = funcs[++tracker];
  }

  SortedArrayOpUnary unary_funcs[] = {SortedArray_Min, SortedArray_Max,
                                      SortedArray_Select_Unary, NULL};
  TestUnaryFunckNullParameter(unary_funcs);

  free(item);
  Array_Destroy(sut);
}

static void SortedArray_null_parameter() {
  SortedArrayOp funcs[] = {SortedArray_Search, SortedArray_Successor,
                           SortedArray_Predecessor, SortedArray_Rank_ArrayOp,
                           NULL};
  TestFunckNullParameter(funcs);

  SortedArrayOpUnary unary_funcs[] = {SortedArray_Min, SortedArray_Max,
                                      SortedArray_Select_Unary, NULL};
  TestUnaryFunckNullParameter(unary_funcs);
}

static void SortedArray_Search_not_found() {
  Array* sut = CreateSut(10);
  // if the first item is 0, this could wrap but the risk is minmal
  int not_there = ((int*)sut->array)[0] - 1;

  void* result = SortedArray_Search(sut, &not_there);
  CU_ASSERT_PTR_NULL(result);

  Array_Destroy(sut);
}

static void SortedArray_Search_standard() {
  const size_t n = 15;

  Array* sut = CreateSut(n);

  for (size_t i = 0; i < n; i++) {
    int there = ((int*)sut->array)[i];
    void* result = SortedArray_Search(sut, &there);
    CU_ASSERT_PTR_EQUAL(result, (char*)sut->array + sizeof(int) * i);
  }

  Array_Destroy(sut);
}

static void SortedArray_Min_empty() {
  ErrorReporter_Clear();
  Array* sut = CreateSut(0);

  void* result = SortedArray_Min(sut);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(kkEmptyList, ErrorReporter_LastErrorCode());

  Array_Destroy(sut);
}

static void SortedArray_Min_standard() {
  Array* sut = CreateSut(10);

  void* result = SortedArray_Min(sut);
  CU_ASSERT_EQUAL(sut->array, result);

  Array_Destroy(sut);
}

static void SortedArray_Max_empty() {
  ErrorReporter_Clear();
  Array* sut = CreateSut(0);

  void* result = SortedArray_Max(sut);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(kkEmptyList, ErrorReporter_LastErrorCode());

  Array_Destroy(sut);
}

static void SortedArray_Max_standard() {
  Array* sut = CreateSut(10);

  void* result = SortedArray_Max(sut);
  int* arr = sut->array;

  CU_ASSERT_EQUAL(&arr[9], result);

  Array_Destroy(sut);
}

static void SortedArray_Predecessor_not_found() {
  Array* sut = CreateSut(10);
  int not_found = *(int*)sut->array - 1;

  ErrorReporter_Clear();
  void* result = SortedArray_Predecessor(sut, &not_found);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(kkNotFound, ErrorReporter_LastErrorCode());

  Array_Destroy(sut);
}

static void SortedArray_Predecessor_empty() {
  Array* sut = CreateSut(0);
  int item = 10;

  ErrorReporter_Clear();
  void* result = SortedArray_Predecessor(sut, &item);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(kkEmptyList, ErrorReporter_LastErrorCode());

  Array_Destroy(sut);
}

static void SortedArray_Predecessor_standard() {
  const size_t n = 10;
  Array* sut = CreateSut(n);
  int* array_t = (int*)sut->array;

  for (size_t i = 1; i < n; i++) {
    int search_for = array_t[i];

    void* result = SortedArray_Predecessor(sut, &search_for);
    CU_ASSERT_PTR_EQUAL(&array_t[i - 1], result);
  }

  Array_Destroy(sut);
}

static void SortedArray_Predecessor_first_item() {
  Array* sut = CreateSut(10);

  int search_for = *(int*)sut->array;

  void* result = SortedArray_Predecessor(sut, &search_for);
  CU_ASSERT_PTR_NULL(result);

  Array_Destroy(sut);
}

static void SortedArray_Successor_not_found() {
  Array* sut = CreateSut(10);
  int not_found = *(int*)sut->array - 1;

  ErrorReporter_Clear();
  void* result = SortedArray_Successor(sut, &not_found);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(kkNotFound, ErrorReporter_LastErrorCode());

  Array_Destroy(sut);
}

static void SortedArray_Successor_empty() {
  Array* sut = CreateSut(0);
  int item = 10;

  ErrorReporter_Clear();
  void* result = SortedArray_Successor(sut, &item);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(kkEmptyList, ErrorReporter_LastErrorCode());

  Array_Destroy(sut);
}

static void SortedArray_Successor_standard() {
  const size_t n = 10;
  Array* sut = CreateSut(n);
  int* array_t = (int*)sut->array;

  for (size_t i = 0; i < n - 1; i++) {
    int search_for = array_t[i];

    void* result = SortedArray_Successor(sut, &search_for);
    CU_ASSERT_PTR_EQUAL(&array_t[i + 1], result);
  }

  Array_Destroy(sut);
}

static void SortedArray_Successor_last_item() {
  const size_t n = 10;
  Array* sut = CreateSut(n);

  int search_for = ((int*)sut->array)[n - 1];

  void* result = SortedArray_Successor(sut, &search_for);
  CU_ASSERT_PTR_NULL(result);

  Array_Destroy(sut);
}

static void SortedArray_Select_out_of_bounds() {
  const size_t n = 10;
  Array* sut = CreateSut(n);

  ErrorReporter_Clear();
  void* result = SortedArray_Select(sut, n);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(kkInvalidIndex, ErrorReporter_LastErrorCode());
  Array_Destroy(sut);
}

static void SortedArray_Select_standard() {
  const size_t n = 10;
  Array* sut = CreateSut(n);
  int* array_t = (int*)sut->array;

  for (size_t i = 0; i < n; i++) {
    void* result = SortedArray_Select(sut, i);
    CU_ASSERT_PTR_EQUAL(&array_t[i], result);
  }

  Array_Destroy(sut);
}

static void SortedArray_Rank_not_found() {
  Array* sut = CreateSut(10);
  int not_found = *(int*)sut->array - 1;

  ErrorReporter_Clear();
  size_t result = SortedArray_Rank(sut, &not_found);
  CU_ASSERT_EQUAL(RANK_ERROR, result);
  CU_ASSERT_EQUAL(kkNotFound, ErrorReporter_LastErrorCode());

  Array_Destroy(sut);
}

static void SortedArray_Rank_empty() {
  Array* sut = CreateSut(0);
  int item = 10;

  ErrorReporter_Clear();
  size_t result = SortedArray_Rank(sut, &item);
  CU_ASSERT_EQUAL(RANK_ERROR, result);
  CU_ASSERT_EQUAL(kkEmptyList, ErrorReporter_LastErrorCode());

  Array_Destroy(sut);
}

static void SortedArray_Rank_standard() {
  const size_t n = 10;
  Array* sut = CreateSut(n);
  int* array_t = (int*)sut->array;

  for (size_t i = 0; i < n; i++) {
    int search_for = array_t[i];
    size_t result = SortedArray_Rank(sut, &search_for);
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
