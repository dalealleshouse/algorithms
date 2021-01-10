/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "quick_select.h"

#include <stdlib.h>
#include <time.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "sorting_test_helpers.h"
#include "test_helpers.h"

static int int_comparator(const void* x, const void* y) {
  if (x == y) return 0;

  int _x = *(int*)x;
  int _y = *(int*)y;

  if (_x == _y) return 0;

  if (_x < _y) return -1;

  return 1;
}

/************* select ****************/
static void select_null() {
  void* result = NULL;

  ResultCode result_code = SortSelect(0, 0, 0, NULL, NULL, &result);
  CU_ASSERT_EQUAL(kNullParameter, result_code);
  CU_ASSERT_EQUAL(NULL, result);
}

static void select_pre_sorted() {
  const size_t n = 6;
  const size_t nth = 2;
  const int expected = 3;
  int arr[] = {1, 2, 3, 4, 5, 6};

  void* result = NULL;
  ResultCode result_code =
      SortSelect(nth, n, sizeof(arr[0]), arr, int_comparator, &result);

  CU_ASSERT_EQUAL(kSuccess, result_code);
  CU_ASSERT_PTR_NOT_NULL_FATAL(result);
  CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0])));  // NOLINT
}

static void select_reversed() {
  const size_t n = 6;
  const size_t nth = 2;
  const int expected = 3;
  int arr[] = {6, 5, 4, 3, 2, 1};

  void* result = NULL;
  ResultCode result_code =
      SortSelect(nth, n, sizeof(arr[0]), arr, int_comparator, &result);

  CU_ASSERT_EQUAL(kSuccess, result_code);
  CU_ASSERT_PTR_NOT_NULL_FATAL(result);
  CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0])));  // NOLINT
}

int RegisterSelectTests() {
  CU_TestInfo partition_tests[] = {
      CU_TEST_INFO(select_null), CU_TEST_INFO(select_reversed),
      CU_TEST_INFO(select_pre_sorted), CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "select suite",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = partition_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}

/************* quick select ****************/
static void QuickSelect_null() {
  void* result = NULL;

  ResultCode result_code = QuickSelect(0, 0, 0, NULL, NULL, &result);
  CU_ASSERT_EQUAL(kNullParameter, result_code);
}

static void QuickSelect_pre_sorted() {
  const size_t n = 6;
  const size_t nth = 3;
  const int expected = 4;
  int arr[] = {1, 2, 3, 4, 5, 6};

  void* result = NULL;
  ResultCode result_code =
      QuickSelect(nth, n, sizeof(arr[0]), arr, int_comparator, &result);

  CU_ASSERT_EQUAL(kSuccess, result_code);
  CU_ASSERT_PTR_NOT_NULL_FATAL(result);
  CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0])));  // NOLINT
}

static void QuickSelect_reversed() {
  const size_t n = 6;
  const size_t nth = 2;
  const int expected = 3;
  int arr[] = {6, 5, 4, 3, 2, 1};

  void* result = NULL;
  ResultCode result_code =
      QuickSelect(nth, n, sizeof(arr[0]), arr, int_comparator, &result);

  CU_ASSERT_EQUAL(kSuccess, result_code);
  CU_ASSERT_PTR_NOT_NULL_FATAL(result);
  CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0])));  // NOLINT
}

static void quick_nth_equals_zero() {
  const size_t n = 6;
  const size_t nth = 0;
  const int expected = 1;
  int arr[] = {3, 2, 1, 6, 5, 4};

  void* result = NULL;
  ResultCode result_code =
      QuickSelect(nth, n, sizeof(arr[0]), arr, int_comparator, &result);

  CU_ASSERT_EQUAL(kSuccess, result_code);
  CU_ASSERT_PTR_NOT_NULL_FATAL(result);
  CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0])));  // NOLINT
}

static void quick_nth_equals_n() {
  const size_t n = 6;
  const size_t nth = 5;
  const int expected = 6;
  int arr[] = {3, 2, 1, 6, 5, 4};

  void* result = NULL;
  ResultCode result_code =
      QuickSelect(nth, n, sizeof(arr[0]), arr, int_comparator, &result);

  CU_ASSERT_EQUAL(kSuccess, result_code);
  CU_ASSERT_PTR_NOT_NULL_FATAL(result);
  CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0])));  // NOLINT
}

static void QuickSelect_nth_out_of_bounds() {
  const size_t n = 6;
  const size_t nth = 6;
  int arr[] = {3, 2, 1, 6, 5, 4};

  void* result = NULL;
  ResultCode result_code =
      QuickSelect(nth, n, sizeof(arr[0]), arr, int_comparator, &result);

  CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
  CU_ASSERT_PTR_NULL_FATAL(result);
}

static void QuickSelect_matches_select() {
  unsigned int seed = time(NULL);
  const size_t n = 100000;
  const size_t nth = rand_r(&seed) % n;
  int s[n];
  int qs[n];

  for (size_t i = 0; i < n; i++) {
    int num = rand_r(&seed);
    s[i] = num;
    qs[i] = num;
  }

  void* s_result = NULL;
  void* qs_result = NULL;

  ResultCode result_code =
      SortSelect(nth, n, sizeof(s[0]), s, int_comparator, &s_result);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code =
      QuickSelect(nth, n, sizeof(qs[0]), s, int_comparator, &qs_result);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  CU_ASSERT_EQUAL(0, memcmp(s_result, qs_result, sizeof(s[0])));
}

static void QuickSelect_find_median_value() {
  const size_t n = 10000;
  const size_t nth = 5000;
  int* result = NULL;
  int* test_data = GenerateTestData();

  ResultCode result_code = QuickSelect(nth, n, sizeof(test_data[0]), test_data,
                                       int_comparator, (void**)&result);
  CU_ASSERT_EQUAL(kSuccess, result_code);
  CU_ASSERT_EQUAL(5001, *result);

  free(test_data);
}

int RegisterQuickSelectTests() {
  CU_TestInfo partition_tests[] = {CU_TEST_INFO(QuickSelect_null),
                                   CU_TEST_INFO(QuickSelect_pre_sorted),
                                   CU_TEST_INFO(quick_nth_equals_zero),
                                   CU_TEST_INFO(quick_nth_equals_n),
                                   CU_TEST_INFO(QuickSelect_nth_out_of_bounds),
                                   CU_TEST_INFO(QuickSelect_matches_select),
                                   CU_TEST_INFO(QuickSelect_reversed),
                                   CU_TEST_INFO(QuickSelect_find_median_value),
                                   CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "quick select suite",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = partition_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
