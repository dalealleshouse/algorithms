#include "./running_median.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "../utils/error_reporter.h"
#include "../utils/malloc_test_wrapper.h"
#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

static const double kEpsilon = 0.0000000000000000001;

#define SUT(code_block)                          \
  {                                              \
    RunningMedian* sut = RunningMedian_Create(); \
    code_block;                                  \
    RunningMedian_Destroy(sut);                  \
  }

static void RunningMedian_Create_failed_mem_allocation() {
#if !defined(NDEBUG)
  ErrorReporter_Clear();
  RunningMedian* sut;

  FAILED_MALLOC_TEST({ sut = RunningMedian_Create(); });
  CU_ASSERT_PTR_NULL(sut);
  CU_ASSERT_EQUAL(kFailedMemoryAllocation, ErrorReporter_LastErrorCode());
#endif
}

static void RunningMedian_Create_happy_path() {
  ErrorReporter_Clear();

  RunningMedian* sut = RunningMedian_Create();
  CU_ASSERT_PTR_NOT_NULL(sut);
  CU_ASSERT_EQUAL(0, ErrorReporter_LastErrorCode());

  RunningMedian_Destroy(sut);
}

static void RunningMedian_Insert_null_parameter() {
  Result result = RunningMedian_Insert(NULL, 0.0);
  CU_ASSERT_EQUAL(kNullParameter, result);
}

static void RunningMedian_Insert_failed_mem_allocation() {
#if !defined(NDEBUG)
  SUT({
    ErrorReporter_Clear();
    FAILED_MALLOC_TEST({
      Result result = RunningMedian_Insert(sut, 5.0);
      CU_ASSERT_EQUAL(kFailedMemoryAllocation, result);
    });
  });
#endif
}

static void RunningMedian_Insert_nan_or_inf() {
  SUT({
    Result result = RunningMedian_Insert(sut, NAN);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result);

    result = RunningMedian_Insert(sut, INFINITY);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result);
  });
}

static void RunningMedian_Insert_happy_path() {
  SUT({
    for (size_t i = 1; i <= 10; i++) {
      Result result = RunningMedian_Insert(sut, i);
      CU_ASSERT_EQUAL(kSuccess, result);

      CU_ASSERT_EQUAL(i, RunningMedian_GetN(sut));
    }
  });
}

static void RunningMedian_GetN_null_parameter() {
  size_t size = RunningMedian_GetN(NULL);
  CU_ASSERT_EQUAL(0, size);
}

static void RunningMedian_Median_null_parameter() {
  ErrorReporter_Clear();
  double result = RunningMedian_Median(NULL);
  CU_ASSERT_TRUE(isnan(result));
  CU_ASSERT_EQUAL(kNullParameter, ErrorReporter_LastErrorCode());
}

static void RunningMedian_Median_empty() {
  SUT({
    ErrorReporter_Clear();
    double result = RunningMedian_Median(sut);
    CU_ASSERT_TRUE(isnan(result));
    CU_ASSERT_EQUAL(kEmpty, ErrorReporter_LastErrorCode());
  });
}

static void RunningMedian_Median_one_value() {
  const double expected = 5.5;
  SUT({
    RunningMedian_Insert(sut, expected);
    double result = RunningMedian_Median(sut);
    CU_ASSERT_DOUBLE_EQUAL(expected, result, kEpsilon);
  });
}

static void RunningMedian_Median_two_values() {
  const double expected = 5.5;
  SUT({
    RunningMedian_Insert(sut, 6);
    RunningMedian_Insert(sut, 5);
    double result = RunningMedian_Median(sut);
    CU_ASSERT_FALSE(isnan(result));
    CU_ASSERT_DOUBLE_EQUAL(expected, result, kEpsilon);
  });
}

static void RunningMedian_Median_happy_path() {
  const size_t n = 10;
  double vals[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  double expected[] = {1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5};
  SUT({
    for (size_t i = 0; i < n; i++) {
      RunningMedian_Insert(sut, vals[i]);
      double result = RunningMedian_Median(sut);
      CU_ASSERT_DOUBLE_EQUAL(expected[i], result, kEpsilon);
    }
  });
}

static void RunningMedian_Median_arit_overflow() {
  SUT({
    RunningMedian_Insert(sut, DBL_MAX);
    RunningMedian_Insert(sut, DBL_MAX);

    ErrorReporter_Clear();
    double result = RunningMedian_Median(sut);

    CU_ASSERT_TRUE(isinf(result));
    CU_ASSERT_EQUAL(kArithmeticOverflow, ErrorReporter_LastErrorCode());
  });
}

static void RunningMedian_Stress() {
  const char* PATH = "src/running_median/test_data/median.txt";
  const size_t BUFFER_SIZE = 1024;

  FILE* file = fopen(PATH, "r");
  char line[BUFFER_SIZE];
  double running_total = 0.0;

  SUT({
    while (fgets(line, BUFFER_SIZE, file)) {
      double d;
      sscanf(line, "%lf", &d);

      Result result = RunningMedian_Insert(sut, d);
      CU_ASSERT_EQUAL(kSuccess, result);

      running_total += RunningMedian_Median(sut);
    }
  });
  fclose(file);

  CU_ASSERT_DOUBLE_EQUAL(46853171, running_total, kEpsilon);
}

int RegisterRunningMedianTests() {
  CU_TestInfo create_tests[] = {
      CU_TEST_INFO(RunningMedian_Create_failed_mem_allocation),
      CU_TEST_INFO(RunningMedian_Create_happy_path), CU_TEST_INFO_NULL};

  CU_TestInfo insert_tests[] = {
      CU_TEST_INFO(RunningMedian_Insert_null_parameter),
      CU_TEST_INFO(RunningMedian_Insert_failed_mem_allocation),
      CU_TEST_INFO(RunningMedian_Insert_nan_or_inf),
      CU_TEST_INFO(RunningMedian_Insert_happy_path), CU_TEST_INFO_NULL};

  CU_TestInfo getn_tests[] = {CU_TEST_INFO(RunningMedian_GetN_null_parameter),
                              CU_TEST_INFO_NULL};

  CU_TestInfo median_tests[] = {
      CU_TEST_INFO(RunningMedian_Median_null_parameter),
      CU_TEST_INFO(RunningMedian_Median_empty),
      CU_TEST_INFO(RunningMedian_Median_one_value),
      CU_TEST_INFO(RunningMedian_Median_two_values),
      CU_TEST_INFO(RunningMedian_Median_arit_overflow),
      CU_TEST_INFO(RunningMedian_Median_happy_path),
      CU_TEST_INFO_NULL};

  CU_TestInfo common_tests[] = {CU_TEST_INFO(RunningMedian_Stress),
                                CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "RunningMedian_Create",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = create_tests},
                           {.pName = "RunningMedian_Insert",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = insert_tests},
                           {.pName = "RunningMedian_GetN",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = getn_tests},
                           {.pName = "RunningMedian_Median",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = median_tests},
                           {.pName = "RunningMedian common",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = common_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
