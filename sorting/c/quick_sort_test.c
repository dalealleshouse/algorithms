#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "quick_sort.h"
#include "sorting_test_helpers.h"

/* Test Suite setup and cleanup functions: */
static int init_suite(void) { return 0; }
static int clean_suite(void) { return 0; }

/************* quick sort ****************/

const static size_t n = 200;

STANDARD_SORTING_TESTS(quick_sort)

static void large() { test_int_array(100000, rev_arr); }

int quick_sort_suite()
{
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("quick sort suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        return -1;
    }

    if (ADD_STANDARD_TESTS(pSuite)
        || NULL == CU_add_test(pSuite, "large", large))
        return CU_get_error();

    return 0;
}

/************* partition ****************/
static void partition_null()
{
    int result = partition(0, 0, NULL, NULL, NULL);
    CU_ASSERT_EQUAL(-1, result);
}

static void partition_does_not_chage_sorted()
{
    const int n = 6;
    const int expected[] = { 1, 2, 3, 4, 5, 6 };
    int arr[] = { 1, 2, 3, 4, 5, 6 };
    size_t pivot_index;

    int result = partition(n, sizeof(int), arr, int_comparator, &pivot_index);

    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(pivot_index, 0);
    arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

static void partition_pivot_on_center()
{
    const int n = 6;
    const int expected[] = { 1, 2, 3, 4, 5, 6 };
    int arr[] = { 4, 2, 3, 1, 5, 6 };
    size_t pivot_index;

    int result = partition(n, sizeof(int), arr, int_comparator, &pivot_index);

    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(pivot_index, 3);
    arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

static void partition_reverse_sorted()
{
    const int n = 6;
    const int expected[] = { 1, 5, 4, 3, 2, 6 };

    int arr[] = { 6, 5, 4, 3, 2, 1 };
    size_t pivot_index;

    int result = partition(n, sizeof(int), arr, int_comparator, &pivot_index);

    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(pivot_index, 5);
    arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

static void partition_first_in_correct_position()
{
    const int n = 5;
    const int expected[] = { 1, 5, 4, 3, 2 };

    int arr[] = { 1, 5, 4, 3, 2 };
    size_t pivot_index;

    int result = partition(n, sizeof(int), arr, int_comparator, &pivot_index);

    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(pivot_index, 0);
    arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

int partition_suite()
{
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("partition suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        return -1;
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "null does not throw", partition_null)
            || NULL
                == CU_add_test(pSuite,
                       "partition does not modify pre sorted array",
                       partition_does_not_chage_sorted)
            || NULL
                == CU_add_test(pSuite, "partition pivot on center item",
                       partition_pivot_on_center)
            || NULL
                == CU_add_test(pSuite,
                       "partition pivot on reverse sorted array",
                       partition_reverse_sorted)
            || NULL
                == CU_add_test(pSuite,
                       "partition does not change when first item is in "
                       "correct posoition",
                       partition_first_in_correct_position)
            /* del this comment */
            )

    ) {
        return CU_get_error();
    }

    return 0;
}
