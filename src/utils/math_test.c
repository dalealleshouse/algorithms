#include "./math.h"
#include "./test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

static const double GRANULARITY = 0.00000000000000000001;

static void Min_SingleValueReturnsValue() {
  int result = Min(1, 5);
  CU_ASSERT_EQUAL(result, 5);
}

static void Min_TwoValuesReturnsLowest() {
  int result = Min(2, 10, 5);
  CU_ASSERT_EQUAL(result, 5);
}

static void Min_NegValueReturnsLowest() {
  int result = Min(3, 10, -5, 5);
  CU_ASSERT_EQUAL(result, -5);
}

static void MinDouble_SingleValueReturnsValue() {
  double result = MinDouble(1, 5.0);
  CU_ASSERT_DOUBLE_EQUAL(result, 5.0, GRANULARITY);
}

static void MinDouble_TwoValuesReturnsLowest() {
  double result = MinDouble(2, 10.8, 5.5);
  CU_ASSERT_EQUAL(result, 5.5);
}

static void MinDouble_NegValueRetunrsLowest() {
  double result = MinDouble(3, 10.4, -5.3, 5.2);
  CU_ASSERT_EQUAL(result, -5.3);
}

static void MaxPArray_ReturnsMaxValueInArray() {
  const size_t n = 6;
  const int expected = 6;
  int arr[] = {6, 5, 4, 3, 2, 1};

  void* result = MaxPArray(n, sizeof(arr[0]), arr, PIntComparator);

  CU_ASSERT_PTR_NOT_NULL_FATAL(result);
  CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0])));  // NOLINT
}

static void MinPArray_RetunrsMinValueInArray() {
  const size_t n = 6;
  const int expected = 1;
  int arr[] = {6, 5, 4, 3, 2, 1};

  void* result = MinPArray(n, sizeof(arr[0]), arr, PIntComparator);

  CU_ASSERT_PTR_NOT_NULL_FATAL(result);
  CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0])));  // NOLINT
}

int RegisterMathTests() {
  CU_TestInfo min_max_tests[] = {
      CU_TEST_INFO(Min_SingleValueReturnsValue),
      CU_TEST_INFO(Min_TwoValuesReturnsLowest),
      CU_TEST_INFO(Min_NegValueReturnsLowest),
      CU_TEST_INFO(MinDouble_SingleValueReturnsValue),
      CU_TEST_INFO(MinDouble_TwoValuesReturnsLowest),
      CU_TEST_INFO(MinDouble_NegValueRetunrsLowest),
      CU_TEST_INFO(MaxPArray_ReturnsMaxValueInArray),
      CU_TEST_INFO(MinPArray_RetunrsMinValueInArray),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "math - Min and Max",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = min_max_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
