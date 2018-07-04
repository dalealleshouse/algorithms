#include <stdio.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "inversions.h"

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
    count_inversions(NULL, 0, sizeof(int), int_comparator);
}

void count_single_inversion(void)
{
    const int size = 4;
    int arr[] = { 1, 2, 4, 3 };

    int result = count_inversions(arr, size, sizeof(int), int_comparator);

    CU_ASSERT_EQUAL(result, 1);
}

void count_two_inversion(void)
{
    const int size = 4;
    int arr[] = { 2, 1, 4, 3 };

    int result = count_inversions(arr, size, sizeof(int), int_comparator);

    CU_ASSERT_EQUAL(result, 2);
}

void count_reverse_sorted_array()
{
    const int size = 6;
    int arr[] = { 6, 5, 4, 3, 2, 1 };

    int result = count_inversions(arr, size, sizeof(int), int_comparator);

    CU_ASSERT_EQUAL(result, 15);
}

void count_odd_sized_array()
{
    const int size = 5;
    int arr[] = { 5, 4, 3, 2, 1 };

    int result = count_inversions(arr, size, sizeof(int), int_comparator);

    CU_ASSERT_EQUAL(result, 10);
}

void count_sorted_array()
{
    const int size = 6;
    int arr[] = { 1, 2, 3, 4, 5, 6 };

    int result = count_inversions(arr, size, sizeof(int), int_comparator);

    CU_ASSERT_EQUAL(result, 0);
}

void counts_structs(void)
{
    const int size = 6;
    test_struct_t arr[6] = { { 0, 0, 6 }, { 0, 0, 5 }, { 0, 0, 4 }, { 0, 0, 3 },
        { 0, 0, 2 }, { 0, 0, 1 } };

    int result
        = count_inversions(arr, size, sizeof(test_struct_t), struct_comparator);

    CU_ASSERT_EQUAL(result, 15);
}

void sorts_pointers(void)
{
    const int size = 6;
    test_struct_t* arr[6];

    for (int i = 0; i < size; i++) {
        arr[i] = malloc(sizeof(test_struct_t));
        arr[i]->sorter = size - i;
    }

    int result = count_inversions(
        arr, size, sizeof(test_struct_t*), *pointer_comparator);

    CU_ASSERT_EQUAL(result, 15);
}

void does_not_count_equal_items()
{
    const int size = 5;
    int arr[] = { 1, 3, 3, 3, 5 };

    int result = count_inversions(arr, size, sizeof(int), int_comparator);

    CU_ASSERT_EQUAL(result, 0);
}

void massive_array()
{
    // Very little error checking here b/c this is just a test and I know what's
    // in the input file
    size_t n = 100000;
    size_t tracker = 0;
    int arr[n];
    char line[256];
    FILE* file = fopen("/src/IntegerArray.txt", "r");

    while (fgets(line, sizeof(line), file) != NULL) {
        arr[tracker] = atoi(line);
        tracker++;
    }

    fclose(file);

    unsigned long result
        = count_inversions(arr, n, sizeof(int), int_comparator);

    CU_ASSERT_EQUAL(result, 2407905288);
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
               == CU_add_test(pSuite, "counts a single inversion",
                      count_single_inversion))
        || (NULL
               == CU_add_test(
                      pSuite, "count two inversions", count_two_inversion))
        || (NULL
               == CU_add_test(pSuite, "counts reverse sorted array",
                      count_reverse_sorted_array))
        || (NULL
               == CU_add_test(
                      pSuite, "counts odd sized array", count_odd_sized_array))
        || (NULL
               == CU_add_test(pSuite, "count sorted array", count_sorted_array))
        || (NULL == CU_add_test(pSuite, "counts structs", counts_structs))
        || (NULL
               == CU_add_test(pSuite, "sorts a pointer array", sorts_pointers))
        || (NULL == CU_add_test(pSuite, "massive array", massive_array))
        || (NULL
               == CU_add_test(pSuite, "does not count equal items",
                      does_not_count_equal_items))) {
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
