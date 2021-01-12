#include "overflow_checker.h"

#include <limits.h>
#include <stdint.h>

#include "./test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

static void IsMulOverflow_size_t_zero() {
  CU_ASSERT_FALSE(IsMulOverflow_size_t(5, 0));
  CU_ASSERT_FALSE(IsMulOverflow_size_t(0, 5));
}

static void IsMulOverflow_size_t_no_overflow() {
  CU_ASSERT_FALSE(IsMulOverflow_size_t(200, 200));
  CU_ASSERT_FALSE(IsMulOverflow_size_t(SIZE_MAX, 1));
}

static void IsMulOverflow_size_t_overflow() {
  CU_ASSERT_TRUE(IsMulOverflow_size_t(SIZE_MAX, 2));
}

static void IsAddOverflow_size_t_no_overflow() {
  CU_ASSERT_FALSE(IsAddOverflow_size_t(5, 5));
  CU_ASSERT_FALSE(IsAddOverflow_size_t(SIZE_MAX - 1, 1));
}

static void IsAddOverflow_size_t_overflow() {
  CU_ASSERT_TRUE(IsAddOverflow_size_t(SIZE_MAX, 1));
}

static void IsAddOverflow_int_no_overflow() {
  CU_ASSERT_FALSE(IsAddOverflow_int(5, 5));
  CU_ASSERT_FALSE(IsAddOverflow_int(INT_MAX - 1, 1));
}

static void IsAddOverflow_int_overflow() {
  CU_ASSERT_TRUE(IsAddOverflow_int(INT_MAX, 1));
  CU_ASSERT_TRUE(IsAddOverflow_int(INT_MIN, -1));
}

static void IsAddOverflow_uint_no_overflow() {
  CU_ASSERT_FALSE(IsAddOverflow_uint(5, 5));
  CU_ASSERT_FALSE(IsAddOverflow_int(UINT_MAX - 1, 1));
}

static void IsAddOverflow_uint_overflow() {
  CU_ASSERT_TRUE(IsAddOverflow_uint(UINT_MAX, 1));
}

static void IsAddOverflow_ulong_no_overflow() {
  CU_ASSERT_FALSE(IsAddOverflow_ulong(5, 5));
  CU_ASSERT_FALSE(IsAddOverflow_ulong(ULONG_MAX - 1, 1));
}

static void IsAddOverflow_ulong_overflow() {
  CU_ASSERT_TRUE(IsAddOverflow_ulong(ULONG_MAX, 1));
}

static void IsMulOverflow_ulong_no_overflow() {
  CU_ASSERT_FALSE(IsMulOverflow_ulong(5, 5));
  CU_ASSERT_FALSE(IsMulOverflow_ulong(ULONG_MAX / 2, 2));
}

static void IsMulOverflow_ulong_overflow() {
  CU_ASSERT_TRUE(IsMulOverflow_ulong(ULONG_MAX / 2, 3));
}

int RegisterOverflowTests() {
  CU_TestInfo size_t_tests[] = {CU_TEST_INFO(IsMulOverflow_size_t_zero),
                                CU_TEST_INFO(IsMulOverflow_size_t_no_overflow),
                                CU_TEST_INFO(IsMulOverflow_size_t_overflow),
                                CU_TEST_INFO(IsAddOverflow_size_t_overflow),
                                CU_TEST_INFO(IsAddOverflow_size_t_no_overflow),
                                CU_TEST_INFO_NULL};

  CU_TestInfo int_tests[] = {CU_TEST_INFO(IsAddOverflow_int_overflow),
                             CU_TEST_INFO(IsAddOverflow_int_no_overflow),
                             CU_TEST_INFO(IsAddOverflow_uint_overflow),
                             CU_TEST_INFO(IsAddOverflow_uint_no_overflow),
                             CU_TEST_INFO_NULL};

  CU_TestInfo long_tests[] = {CU_TEST_INFO(IsAddOverflow_ulong_overflow),
                              CU_TEST_INFO(IsAddOverflow_ulong_no_overflow),
                              CU_TEST_INFO(IsMulOverflow_ulong_overflow),
                              CU_TEST_INFO(IsMulOverflow_ulong_no_overflow),
                              CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "size_t_tests",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = size_t_tests},
                           {.pName = "int_tests",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = int_tests},
                           {.pName = "long",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = long_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
