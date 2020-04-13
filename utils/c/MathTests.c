#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "TestHelpers.h"

#include "Math.h"

static const double GRANULARITY = 0.00000000000000000001;

static void min_single_value() {
  int result = min(1, 5);
  CU_ASSERT_EQUAL(result, 5);
}

static void min_two_value() {
  int result = min(2, 10, 5);
  CU_ASSERT_EQUAL(result, 5);
}

static void min_neg_value() {
  int result = min(3, 10, -5, 5);
  CU_ASSERT_EQUAL(result, -5);
}

static void min_double_single_value() {
  double result = min_double(1, 5.0);
  CU_ASSERT_DOUBLE_EQUAL(result, 5.0, GRANULARITY);
}

static void min_double_two_value() {
  double result = min_double(2, 10.8, 5.5);
  CU_ASSERT_EQUAL(result, 5.5);
}

static void min_double_neg_value() {
  double result = min_double(3, 10.4, -5.3, 5.2);
  CU_ASSERT_EQUAL(result, -5.3);
}

int registerMathTests() {
  CU_TestInfo minTests[] = {CU_TEST_INFO(min_single_value),
                            CU_TEST_INFO(min_two_value),
                            CU_TEST_INFO(min_neg_value),
                            CU_TEST_INFO(min_double_single_value),
                            CU_TEST_INFO(min_double_two_value),
                            CU_TEST_INFO(min_double_neg_value),
                            CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "min",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = minTests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
