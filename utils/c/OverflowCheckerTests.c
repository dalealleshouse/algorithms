#include <stdint.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "TestHelpers.h"

#include "OverflowChecker.h"

static void is_mul_overflow_size_t_zero()
{
    CU_ASSERT_FALSE(is_mul_overflow_size_t(5, 0));
    CU_ASSERT_FALSE(is_mul_overflow_size_t(0, 5));
}

static void is_mul_overflow_size_t_no_overflow()
{
    CU_ASSERT_FALSE(is_mul_overflow_size_t(200, 200));
    CU_ASSERT_FALSE(is_mul_overflow_size_t(SIZE_MAX, 1));
}

static void is_mul_overflow_size_t_overflow()
{
    CU_ASSERT_TRUE(is_mul_overflow_size_t(SIZE_MAX, 2));
}

static void is_add_overflow_size_t_no_overflow()
{
    CU_ASSERT_FALSE(is_add_overflow_size_t(5, 5));
    CU_ASSERT_FALSE(is_add_overflow_size_t(SIZE_MAX - 1, 1));
}

static void is_add_overflow_size_t_overflow()
{
    CU_ASSERT_TRUE(is_add_overflow_size_t(SIZE_MAX, 1));
}

int register_overflow_tests()
{
    CU_TestInfo Queue_tests[] = { CU_TEST_INFO(is_mul_overflow_size_t_zero),
        CU_TEST_INFO(is_mul_overflow_size_t_no_overflow),
        CU_TEST_INFO(is_mul_overflow_size_t_overflow),
        CU_TEST_INFO(is_add_overflow_size_t_overflow),
        CU_TEST_INFO(is_add_overflow_size_t_no_overflow), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "is_mul_overflow_size_t",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Queue_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
