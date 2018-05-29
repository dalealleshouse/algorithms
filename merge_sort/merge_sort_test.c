#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "merge_sort.h"

#include <stdio.h> // for printf

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* Test case functions ****************/

int int_comparator(const void* x, const void* y)
{
    int _x = *(int*)x;
    int _y = *(int*)y;

    if (x == y)
        return 0;

    if (_x < _y)
        return -1;

    return 1;
}

void null_does_not_throw_test(void)
{
    sort(NULL, NULL, 0, sizeof(int), int_comparator);
}

void sorts_even_size_array(void)
{
    int arr[] = { 6, 5, 4, 3, 2, 1 };
    int sorted[6];

    int result = sort(arr, sorted, 6, sizeof(int), int_comparator);

    for (int i = 0; i < 6; i++)
        CU_ASSERT_EQUAL(sorted[i], i + 1);

    CU_ASSERT_EQUAL(result, 0);
}

void test_case_sample(void)
{
    CU_ASSERT(CU_TRUE);
    CU_ASSERT_NOT_EQUAL(2, -1);
    CU_ASSERT_STRING_EQUAL("string #1", "string #1");
    CU_ASSERT_STRING_NOT_EQUAL("string #1", "string #2");

    CU_ASSERT(CU_TRUE);
    CU_ASSERT_EQUAL(2, 2);
    CU_ASSERT_STRING_NOT_EQUAL("string #1", "string #2");
    CU_ASSERT_STRING_EQUAL("string #1", "string #1");
}

/************* Test Runner Code goes here **************/

int main(void)
{
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("merge_sort_test_suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if ((NULL
            == CU_add_test(pSuite, "null input does not cause error",
                   null_does_not_throw_test))
        || (NULL
               == CU_add_test(pSuite, "sort of even sized array invalid",
                      sorts_even_size_array))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    /* printf("\n"); */
    /* CU_basic_show_failures(CU_get_failure_list()); */
    /* printf("\n\n"); */

    /* Clean up registry and return */
    CU_cleanup_registry();
    return CU_get_error();
}
