#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "insertion_sort.h"
#include "sorting_test_helpers.h"

const static size_t n = 200;

/* Test Suite setup and cleanup functions: */
static int init_suite(void) { return 0; }
static int clean_suite(void) { return 0; }

STANDARD_SORTING_TESTS(insertion_sort)

int insertion_sort_suite()
{
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("insertion sort suite", init_suite, clean_suite);
    if (NULL == pSuite)
        return -1;

    if (ADD_STANDARD_TESTS(pSuite))
        return CU_get_error();

    return 0;
}
