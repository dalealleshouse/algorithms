#include <stdlib.h>
#include <time.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "quick_select.h"

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int int_comparator(const void* x, const void* y)
{
    if (x == y)
        return 0;

    int _x = *(int*)x;
    int _y = *(int*)y;

    if (_x == _y)
        return 0;

    if (_x < _y)
        return -1;

    return 1;
}

/* Test Suite setup and cleanup functions: */
static int init_suite(void) { return 0; }
static int clean_suite(void) { return 0; }

/************* select ****************/
static void select_null()
{
    void* result = sort_select(0, 0, 0, NULL, NULL);
    CU_ASSERT_EQUAL(NULL, result);
}

static void select_pre_sorted()
{
    const size_t n = 6;
    const size_t nth = 2;
    const int expected = 3;
    int arr[] = { 1, 2, 3, 4, 5, 6 };

    void* result = sort_select(nth, n, sizeof(arr[0]), arr, int_comparator);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0]))); // NOLINT
}

static void select_reversed()
{
    const size_t n = 6;
    const size_t nth = 2;
    const int expected = 3;
    int arr[] = { 6, 5, 4, 3, 2, 1 };

    void* result = sort_select(nth, n, sizeof(arr[0]), arr, int_comparator);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0]))); // NOLINT
}

static int select_suite()
{
    CU_TestInfo partition_tests[]
        = { CU_TEST_INFO(select_null), CU_TEST_INFO(select_reversed),
              CU_TEST_INFO(select_pre_sorted), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "select suite",
                                  .pInitFunc = init_suite,
                                  .pCleanupFunc = clean_suite,
                                  .pTests = partition_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}

/************* quick select ****************/
static void quick_select_null()
{
    void* result = quick_select(0, 0, 0, NULL, NULL);
    CU_ASSERT_EQUAL(NULL, result);
}

static void quick_select_pre_sorted()
{
    const size_t n = 6;
    const size_t nth = 3;
    const int expected = 4;
    int arr[] = { 1, 2, 3, 4, 5, 6 };

    void* result = quick_select(nth, n, sizeof(arr[0]), arr, int_comparator);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0]))); // NOLINT
}

static void quick_select_reversed()
{
    const size_t n = 6;
    const size_t nth = 2;
    const int expected = 3;
    int arr[] = { 6, 5, 4, 3, 2, 1 };

    void* result = quick_select(nth, n, sizeof(arr[0]), arr, int_comparator);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0]))); // NOLINT
}

static void quick_nth_equals_zero()
{
    const size_t n = 6;
    const size_t nth = 0;
    const int expected = 1;
    int arr[] = { 3, 2, 1, 6, 5, 4 };

    void* result = quick_select(nth, n, sizeof(arr[0]), arr, int_comparator);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0]))); // NOLINT
}

static void quick_nth_equals_n()
{
    const size_t n = 6;
    const size_t nth = 5;
    const int expected = 6;
    int arr[] = { 3, 2, 1, 6, 5, 4 };

    void* result = quick_select(nth, n, sizeof(arr[0]), arr, int_comparator);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0]))); // NOLINT
}

static void quick_select_nth_out_of_bounds()
{
    const size_t n = 6;
    const size_t nth = 6;
    int arr[] = { 3, 2, 1, 6, 5, 4 };

    void* result = quick_select(nth, n, sizeof(arr[0]), arr, int_comparator);

    CU_ASSERT_PTR_NULL_FATAL(result);
}

static void quick_select_matches_select()
{
    const size_t n = 100000;
    const size_t nth = rand() % n;
    int s[n];
    int qs[n];

    for (size_t i = 0; i < n; i++) {
        int num = rand();
        s[i] = num;
        qs[i] = num;
    }

    void* s_result = sort_select(nth, n, sizeof(s[0]), s, int_comparator);
    void* qs_result = quick_select(nth, n, sizeof(qs[0]), s, int_comparator);
    CU_ASSERT_EQUAL(0, memcmp(s_result, qs_result, sizeof(s[0])));
}

static int quick_select_suite()
{
    CU_TestInfo partition_tests[] = { CU_TEST_INFO(quick_select_null),
        CU_TEST_INFO(quick_select_pre_sorted),
        CU_TEST_INFO(quick_nth_equals_zero), CU_TEST_INFO(quick_nth_equals_n),
        CU_TEST_INFO(quick_select_nth_out_of_bounds),
        CU_TEST_INFO(quick_select_matches_select),
        CU_TEST_INFO(quick_select_reversed), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "quick select suite",
                                  .pInitFunc = init_suite,
                                  .pCleanupFunc = clean_suite,
                                  .pTests = partition_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}

/************* partition ****************/
static void partition_null()
{
    int result = partition(0, 0, NULL, NULL);
    CU_ASSERT_EQUAL(-1, result);
}

static void partition_does_not_chage_sorted()
{
    const size_t n = 6;
    const int expected[] = { 1, 2, 3, 4, 5, 6 };
    int arr[] = { 1, 2, 3, 4, 5, 6 };

    int result = partition(n, sizeof(int), arr, int_comparator);

    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(memcmp(expected, arr, n * sizeof(arr[0])), 0)
}

static void partition_pivot_on_center()
{
    const size_t n = 6;
    const int expected[] = { 1, 2, 3, 4, 5, 6 };
    int arr[] = { 4, 2, 3, 1, 5, 6 };

    int result = partition(n, sizeof(int), arr, int_comparator);

    CU_ASSERT_EQUAL(result, 3);
    CU_ASSERT_EQUAL(memcmp(expected, arr, n * sizeof(arr[0])), 0)
}

static void partition_reverse_sorted()
{
    const size_t n = 6;
    const int expected[] = { 1, 5, 4, 3, 2, 6 };

    int arr[] = { 6, 5, 4, 3, 2, 1 };

    int result = partition(n, sizeof(int), arr, int_comparator);

    CU_ASSERT_EQUAL(result, 5);
    CU_ASSERT_EQUAL(memcmp(expected, arr, n * sizeof(arr[0])), 0)
}

static void partition_first_in_correct_position()
{
    const size_t n = 5;
    const int expected[] = { 1, 5, 4, 3, 2 };

    int arr[] = { 1, 5, 4, 3, 2 };

    int result = partition(n, sizeof(int), arr, int_comparator);

    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(memcmp(expected, arr, n * sizeof(arr[0])), 0)
}

static void partition_dup_values()
{
    const size_t n = 6;
    const int expected[] = { 6, 6, 6, 6, 6, 6 };

    int arr[] = { 6, 6, 6, 6, 6, 6 };

    int result = partition(n, sizeof(int), arr, int_comparator);

    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(memcmp(expected, arr, n * sizeof(arr[0])), 0)
}

static void partition_single_value()
{
    const size_t n = 1;
    const int expected[] = { 6 };

    int arr[] = { 6 };

    int result = partition(n, sizeof(int), arr, int_comparator);

    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(memcmp(expected, arr, n * sizeof(arr[0])), 0)
}

static void partition_two_values()
{
    const size_t n = 2;
    const int expected[] = { 5, 6 };

    int arr[] = { 6, 5 };

    int result = partition(n, sizeof(int), arr, int_comparator);

    CU_ASSERT_EQUAL(result, 1);
    CU_ASSERT_EQUAL(memcmp(expected, arr, n * sizeof(arr[0])), 0)
}

static int partition_suite()
{
    CU_TestInfo partition_tests[] = { CU_TEST_INFO(partition_null),
        CU_TEST_INFO(partition_does_not_chage_sorted),
        CU_TEST_INFO(partition_pivot_on_center),
        CU_TEST_INFO(partition_reverse_sorted),
        CU_TEST_INFO(partition_first_in_correct_position),
        CU_TEST_INFO(partition_dup_values),
        CU_TEST_INFO(partition_single_value),
        CU_TEST_INFO(partition_two_values), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "partition suite",
                                  .pInitFunc = init_suite,
                                  .pCleanupFunc = clean_suite,
                                  .pTests = partition_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}

/************* partition ****************/
static void max_test()
{
    const size_t n = 6;
    const int expected = 6;
    int arr[] = { 6, 5, 4, 3, 2, 1 };

    void* result = max(n, sizeof(arr[0]), arr, int_comparator);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0]))); // NOLINT
}

static void min_test()
{
    const size_t n = 6;
    const int expected = 1;
    int arr[] = { 6, 5, 4, 3, 2, 1 };

    void* result = min(n, sizeof(arr[0]), arr, int_comparator);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    CU_ASSERT_EQUAL(0, memcmp(&expected, result, sizeof(arr[0]))); // NOLINT
}

static int min_max_suite()
{
    CU_TestInfo min_max_tests[]
        = { CU_TEST_INFO(max_test), CU_TEST_INFO(min_test), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "min max suite",
                                  .pInitFunc = init_suite,
                                  .pCleanupFunc = clean_suite,
                                  .pTests = min_max_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}

int main(void)
{
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    int result;
    if ((result = select_suite()) != 0 || (result = quick_select_suite()) != 0
        || (result = min_max_suite()) != 0
        || (result = partition_suite() != 0)) {
        CU_cleanup_registry();
        return -1;
    }

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    /* Clean up registry and return */
    CU_cleanup_registry();
    return CU_get_error();
}
