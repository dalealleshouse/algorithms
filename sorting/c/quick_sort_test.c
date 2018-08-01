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

static void null()
{
    int result = quick_sort(0, 0, NULL, NULL);
    CU_ASSERT_EQUAL(-1, result);
}

static void* test_array(const size_t n, const size_t size,
    const arr_gen arr_gen, const comparator comparator)
{
    void* arr = arr_gen(n);
    void* expected = sort_with_c(n, size, arr, comparator);
    int result = quick_sort(n, size, arr, comparator);

    /* test_struct_t* actual = (test_struct_t*)arr; */
    /* test_struct_t* wanted = (test_struct_t*)expected; */
    /* for (size_t i = 0; i < n; i++) { */
    /*     printf("%d %d = %d %d \n", actual[i].foo, actual[i].sorter, */
    /*         wanted[i].foo, wanted[i].sorter); */
    /* } */

    CU_ASSERT_EQUAL(0, result);
    arrays_are_equal(n, size, expected, arr);

    free(expected);
    return arr;
}

static void test_int_array(const size_t n, const arr_gen arr_gen)
{
    free(test_array(n, sizeof(int), rand_arr, int_comparator));
}

static void single_item_array() { test_int_array(1, rand_arr); }

static void small_array() { test_int_array(6, rand_arr); }

static void large_array() { test_int_array(100000, rand_arr); }

static void odd() { test_int_array(5, rand_arr); }

static void standard_case() { test_int_array(n, rand_arr); }

static void pre_sorted() { test_int_array(n, seq_arr); }

static void reversed() { test_int_array(n, rev_arr); }

static void struct_array()
{
    free(test_array(n, sizeof(test_struct_t), rand_st_arr, struct_comparator));
}

static void ptr_array()
{
    void* arr = test_array(n, sizeof(void*), rand_ptr_arr, pointer_comparator);
    free_ptr_arr(n, arr);
}

static void dup_items() { test_int_array(n, dup_val_arr); }

int quick_sort_suite()
{
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("quick sort suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        return -1;
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "null does not throw", null)
            || NULL
                == CU_add_test(pSuite, "single item array", single_item_array)
            || NULL == CU_add_test(pSuite, "small array", small_array)
            || NULL == CU_add_test(pSuite, "large array", large_array)
            || NULL == CU_add_test(pSuite, "standard case", standard_case)
            || NULL == CU_add_test(pSuite, "pre-sorted", pre_sorted)
            || NULL == CU_add_test(pSuite, "reversed", reversed)
            || NULL == CU_add_test(pSuite, "odd sized", odd)
            || NULL == CU_add_test(pSuite, "struct", struct_array)
            || NULL == CU_add_test(pSuite, "pointers", ptr_array)
            || NULL == CU_add_test(pSuite, "duplicate items", dup_items)
            /* del this comment */
            )

    ) {
        return CU_get_error();
    }

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
