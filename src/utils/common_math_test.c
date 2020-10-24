/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "common_math.h"

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "test_helpers.h"

static const double GRANULARITY = 0.00000000000000000001;

static void Min_single_value_returns_value() {
  int result = Min(1, 5);
  CU_ASSERT_EQUAL(result, 5);
}

static void Min_two_values_returns_lowest() {
  int result = Min(2, 10, 5);
  CU_ASSERT_EQUAL(result, 5);
}

static void Min_neg_value_returns_lowest() {
  int result = Min(3, 10, -5, 5);
  CU_ASSERT_EQUAL(result, -5);
}

static void MinDouble_single_value_returns_value() {
  double result = MinDouble(1, 5.0);
  CU_ASSERT_DOUBLE_EQUAL(result, 5.0, GRANULARITY);
}

static void MinDouble_two_values_returns_lowest() {
  double result = MinDouble(2, 10.8, 5.5);
  CU_ASSERT_EQUAL(result, 5.5);
}

static void MinDouble_neg_value_returns_lowest() {
  double result = MinDouble(3, 10.4, -5.3, 5.2);
  CU_ASSERT_EQUAL(result, -5.3);
}

static void MaxPArray_returns_max_value_in_array() {
  const size_t n = 6;
  const int expected = 6;
  int arr[] = {6, 5, 4, 3, 2, 1};

  void* result = MaxPArray(n, sizeof(arr[0]), arr, PIntComparator);

  CU_ASSERT_PTR_NOT_NULL_FATAL(result);
  CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0])));  // NOLINT
}

static void MinPArray_returns_min_value_in_array() {
  const size_t n = 6;
  const int expected = 1;
  int arr[] = {6, 5, 4, 3, 2, 1};

  void* result = MinPArray(n, sizeof(arr[0]), arr, PIntComparator);

  CU_ASSERT_PTR_NOT_NULL_FATAL(result);
  CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0])));  // NOLINT
}

static void NextPowerOfTwo_zero() {
  size_t result = NextPowerOfTwo(0);
  // 2^0 = 1
  CU_ASSERT_EQUAL(result, 1);
}

static void NextPowerOfTwo_returns_value_if_already_pow_2() {
  for (size_t i = 1; i <= 32768; i = i * 2) {
    CU_ASSERT_EQUAL(NextPowerOfTwo(i), i);
  }
}

static void NextPowerOfTwo_happy_path() {
  size_t pow2 = 1;
  for (size_t i = 0; i <= 32768; i++) {
    if (i == pow2) {
      pow2 *= 2;
    } else {
      CU_ASSERT_EQUAL(NextPowerOfTwo(i), pow2);
    }
  }
}

int RegisterMathTests() {
  CU_TestInfo min_max_tests[] = {
      CU_TEST_INFO(Min_single_value_returns_value),
      CU_TEST_INFO(Min_two_values_returns_lowest),
      CU_TEST_INFO(Min_neg_value_returns_lowest),
      CU_TEST_INFO(MinDouble_single_value_returns_value),
      CU_TEST_INFO(MinDouble_two_values_returns_lowest),
      CU_TEST_INFO(MinDouble_neg_value_returns_lowest),
      CU_TEST_INFO(MaxPArray_returns_max_value_in_array),
      CU_TEST_INFO(MinPArray_returns_min_value_in_array),
      CU_TEST_INFO(NextPowerOfTwo_zero),
      CU_TEST_INFO(NextPowerOfTwo_returns_value_if_already_pow_2),
      CU_TEST_INFO(NextPowerOfTwo_happy_path),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "math - Min and Max",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = min_max_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
