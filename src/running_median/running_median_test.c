/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "running_median.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "malloc_test_wrapper.h"
#include "test_helpers.h"

static const double kEpsilon = 0.0000000000000000001;

#define SUT(code_block)                                  \
  {                                                      \
    RunningMedian* sut = NULL;                           \
    ResultCode result_code = RunningMedian_Create(&sut); \
    CU_ASSERT_EQUAL(kSuccess, result_code);              \
    code_block;                                          \
    RunningMedian_Destroy(sut);                          \
  }

static void RunningMedian_Create_failed_mem_allocation() {
#if !defined(NDEBUG)
  ResultCode result_code;
  RunningMedian* sut = NULL;

  FAILED_MALLOC_TEST({
    result_code = RunningMedian_Create(&sut);
    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(kFailedMemoryAllocation, result_code);
  });
#endif
}

static void RunningMedian_Create_happy_path() {
  RunningMedian* sut = NULL;
  ResultCode result_code = RunningMedian_Create(&sut);
  CU_ASSERT_EQUAL(kSuccess, result_code);
  CU_ASSERT_PTR_NOT_NULL(sut);

  RunningMedian_Destroy(sut);
}

static void RunningMedian_Create_null_parameter() {
  ResultCode result_code = RunningMedian_Create(NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  RunningMedian* sut = malloc(1);
  result_code = RunningMedian_Create(&sut);
  CU_ASSERT_EQUAL(kOutputPointerIsNotNull, result_code);

  free(sut);
}

static void RunningMedian_Insert_null_parameter() {
  Result result = RunningMedian_Insert(NULL, 0.0);
  CU_ASSERT_EQUAL(kNullParameter, result);
}

static void RunningMedian_Insert_failed_mem_allocation() {
#if !defined(NDEBUG)
  SUT({
    FAILED_MALLOC_TEST({
      result_code = RunningMedian_Insert(sut, 5.0);
      CU_ASSERT_EQUAL(kFailedMemoryAllocation, result_code);
    });
  });
#endif
}

static void RunningMedian_Insert_nan_or_inf() {
  SUT({
    result_code = RunningMedian_Insert(sut, NAN);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);

    result_code = RunningMedian_Insert(sut, INFINITY);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
  });
}

static void RunningMedian_Insert_happy_path() {
  SUT({
    for (size_t i = 1; i <= 10; i++) {
      result_code = RunningMedian_Insert(sut, i);
      CU_ASSERT_EQUAL(kSuccess, result_code);
      CU_ASSERT_EQUAL(i, RunningMedian_GetN(sut));
    }
  });
}

static void RunningMedian_GetN_null_parameter() {
  size_t size = RunningMedian_GetN(NULL);
  CU_ASSERT_EQUAL(0, size);
}

static void RunningMedian_Median_null_parameter() {
  median_value result;

  ResultCode result_code = RunningMedian_Median(NULL, &result);
  CU_ASSERT_EQUAL(kNullParameter, result_code);
}

static void RunningMedian_Median_empty() {
  SUT({
    median_value result;
    ResultCode result_code = RunningMedian_Median(sut, &result);
    CU_ASSERT_EQUAL(kEmpty, result_code);
  });
}

static void RunningMedian_Median_one_value() {
  const double expected = 5.5;
  SUT({
    median_value result;
    RunningMedian_Insert(sut, expected);
    ResultCode result_code = RunningMedian_Median(sut, &result);
    CU_ASSERT_EQUAL(kSuccess, result_code);
    CU_ASSERT_DOUBLE_EQUAL(expected, result, kEpsilon);
  });
}

static void RunningMedian_Median_two_values() {
  const double expected = 5.5;
  SUT({
    RunningMedian_Insert(sut, 6);
    RunningMedian_Insert(sut, 5);
    median_value result;
    result_code = RunningMedian_Median(sut, &result);
    CU_ASSERT_EQUAL(kSuccess, result_code);
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
      median_value result;
      result_code = RunningMedian_Median(sut, &result);
      CU_ASSERT_EQUAL(kSuccess, result_code);
      CU_ASSERT_DOUBLE_EQUAL(expected[i], result, kEpsilon);
    }
  });
}

static void RunningMedian_Median_arit_overflow() {
  SUT({
    RunningMedian_Insert(sut, DBL_MAX);
    RunningMedian_Insert(sut, DBL_MAX);

    median_value result;
    result_code = RunningMedian_Median(sut, &result);

    CU_ASSERT_TRUE(isinf(result));
    CU_ASSERT_EQUAL(kArithmeticOverflow, result_code);
  });
}

static void RunningMedian_Stress() {
  const char* PATH = "src/running_median/test_data/median.txt";
  const size_t BUFFER_SIZE = 1024;

  FILE* file = fopen(PATH, "r");
  char line[BUFFER_SIZE];
  median_value running_total = 0.0;

  SUT({
    char* end;
    while (fgets(line, BUFFER_SIZE, file)) {
      median_value d = strtol(line, &end, 10);

      result_code = RunningMedian_Insert(sut, d);
      CU_ASSERT_EQUAL(kSuccess, result_code);

      RunningMedian_Median(sut, &d);
      running_total += d;
    }
  });
  fclose(file);

  CU_ASSERT_DOUBLE_EQUAL(46853171, running_total, kEpsilon);
}

int RegisterRunningMedianTests() {
  CU_TestInfo create_tests[] = {
      CU_TEST_INFO(RunningMedian_Create_failed_mem_allocation),
      CU_TEST_INFO(RunningMedian_Create_happy_path),
      CU_TEST_INFO(RunningMedian_Create_null_parameter),
      CU_TEST_INFO(RunningMedian_Insert_null_parameter),
      CU_TEST_INFO(RunningMedian_Insert_failed_mem_allocation),
      CU_TEST_INFO(RunningMedian_Insert_nan_or_inf),
      CU_TEST_INFO(RunningMedian_Insert_happy_path),
      CU_TEST_INFO(RunningMedian_GetN_null_parameter),
      CU_TEST_INFO(RunningMedian_Median_null_parameter),
      CU_TEST_INFO(RunningMedian_Median_empty),
      CU_TEST_INFO(RunningMedian_Median_one_value),
      CU_TEST_INFO(RunningMedian_Median_two_values),
      CU_TEST_INFO(RunningMedian_Median_arit_overflow),
      CU_TEST_INFO(RunningMedian_Median_happy_path),
      CU_TEST_INFO(RunningMedian_Stress),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "RunningMedian",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = create_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
