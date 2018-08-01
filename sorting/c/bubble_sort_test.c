#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "bubble_sort.h"
#include "bubble_sort_test.h"
#include "sorting_test_helpers.h"

const static size_t n = 200;

/* Test Suite setup and cleanup functions: */
static int init_suite(void) { return 0; }
static int clean_suite(void) { return 0; }

static void null()
{
    int result = bubble_sort(0, 0, NULL, NULL);
    CU_ASSERT_EQUAL(-1, result);
}

static void* test_array(const size_t n, const size_t size,
    const arr_gen arr_gen, const comparator comparator)
{
    void* arr = arr_gen(n);
    void* expected = sort_with_c(n, size, arr, comparator);
    int result = bubble_sort(n, size, arr, comparator);

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

static void standard_case() { test_int_array(n, rand_arr); }

static void pre_sorted() { test_int_array(n, seq_arr); }

static void reversed() { test_int_array(n, rev_arr); }

static void dup_items() { test_int_array(n, dup_val_arr); }

static void struct_array()
{
    free(test_array(n, sizeof(test_struct_t), rand_st_arr, struct_comparator));
}

static void ptr_array()
{
    void* arr = test_array(n, sizeof(void*), rand_ptr_arr, pointer_comparator);
    free_ptr_arr(n, arr);
}

int bubble_sort_suite()
{
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("bubble sort suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        return -1;
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "null does not throw", null)
            || NULL
                == CU_add_test(pSuite, "single item array", single_item_array)
            || NULL == CU_add_test(pSuite, "small array", small_array)
            || NULL == CU_add_test(pSuite, "standard case", standard_case)
            || NULL == CU_add_test(pSuite, "pre-sorted", pre_sorted)
            || NULL == CU_add_test(pSuite, "reversed", reversed)
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
