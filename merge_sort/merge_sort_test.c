#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "merge_sort.h"

#include <stdio.h> // for printf

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

typedef struct test_struct {
    int foo;
    int bar;
    char sorter;
} test_struct_t;

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

int struct_comparator(const void* x, const void* y)
{
    if (x == y)
        return 0;

    if (x == NULL && y != NULL)
        return 0;

    if (y == NULL && x != NULL)
        return 1;

    test_struct_t* _x = (test_struct_t*)x;
    test_struct_t* _y = (test_struct_t*)y;

    return int_comparator(&_x->sorter, &_y->sorter);
}

int pointer_comparator(const void* x, const void* y)
{
    if (x == y)
        return 0;

    if (x == NULL && y != NULL)
        return 0;

    if (y == NULL && x != NULL)
        return 1;

    test_struct_t* _x = *(test_struct_t**)x;
    test_struct_t* _y = *(test_struct_t**)y;

    return int_comparator(&_x->sorter, &_y->sorter);
}

/************* Test case functions ****************/

void null_does_not_throw_test(void)
{
    sort(NULL, NULL, 0, sizeof(int), int_comparator);
}

void sorts_even_size_array(void)
{
    const int size = 6;
    int arr[] = { 6, 5, 4, 3, 2, 1 };
    int sorted[size];

    int result = sort(arr, sorted, size, sizeof(int), int_comparator);

    for (int i = 0; i < size; i++)
        CU_ASSERT_EQUAL(sorted[i], i + 1);

    CU_ASSERT_EQUAL(result, 0);
}

void sorts_pre_sorted_array(void)
{
    const int size = 6;
    int arr[] = { 1, 2, 3, 4, 5, 6 };
    int sorted[size];

    int result = sort(arr, sorted, size, sizeof(int), int_comparator);

    for (int i = 0; i < size; i++)
        CU_ASSERT_EQUAL(sorted[i], i + 1);

    CU_ASSERT_EQUAL(result, 0);
}

void sorts_odd_size_array(void)
{
    const int size = 7;
    int arr[] = { 1, 2, 3, 4, 5, 6, 7 };
    int sorted[size];

    int result = sort(arr, sorted, size, sizeof(int), int_comparator);

    for (int i = 0; i < size; i++)
        CU_ASSERT_EQUAL(sorted[i], i + 1);

    CU_ASSERT_EQUAL(result, 0);
}

void sorts_structs(void)
{
    const int size = 6;
    test_struct_t arr[6] = { { 0, 0, 6 }, { 0, 0, 5 }, { 0, 0, 4 },
        { 0, 0, 3 }, { 0, 0, 2 }, { 0, 0, 1 } };
    test_struct_t sorted[6];

    int result
        = sort(arr, sorted, size, sizeof(test_struct_t), struct_comparator);

    CU_ASSERT_EQUAL(result, 0);

    for (int i = 0; i < size; i++)
        CU_ASSERT_EQUAL(sorted[i].sorter, i + 1);
}

void sorts_pointers(void)
{
    const int size = 6;
    test_struct_t* arr[6];
    test_struct_t* sorted[6];

    for (int i = 0; i < size; i++) {
        arr[i] = malloc(sizeof(test_struct_t));
        arr[i]->sorter = i + 1;
    }

    int result
        = sort(arr, sorted, size, sizeof(test_struct_t*), pointer_comparator);

    CU_ASSERT_EQUAL(result, 0);

    for (int i = 0; i < size; i++) {
        CU_ASSERT_EQUAL(sorted[i]->sorter, i + 1);
        free(sorted[i]);
    }
}

void sorts_large_array(void)
{
    const int size = 100000;
    int arr[size];
    int sorted[size];

    for (int i = 0; i < size; i++)
        arr[i] = rand();

    int result = sort(arr, sorted, size, sizeof(int), int_comparator);
    CU_ASSERT_EQUAL(result, 0);

    int prev = -1;
    for (int i = 0; i < size; i++) {
        CU_ASSERT(prev <= sorted[i]);
        prev = sorted[i];
    }
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
               == CU_add_test(pSuite, "sort of even sized array",
                      sorts_even_size_array))
        || (NULL
               == CU_add_test(pSuite, "sorts an array that is already sorted",
                      sorts_pre_sorted_array))
        || (NULL
               == CU_add_test(pSuite, "sorts an odd sized array",
                      sorts_odd_size_array))
        || (NULL == CU_add_test(pSuite, "sorts a struct array", sorts_structs))
        || (NULL
               == CU_add_test(pSuite, "sorts a pointer array", sorts_pointers))
        || (NULL
               == CU_add_test(
                      pSuite, "sorts a large array", sorts_large_array))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    /* Clean up registry and return */
    CU_cleanup_registry();
    return CU_get_error();
}
