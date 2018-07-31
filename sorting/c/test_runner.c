#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "quick_sort_test.h"
#include "bubble_sort_test.h"

int main(void)
{
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    int result = quick_sort_suite();
    if (result != 0) {
        CU_cleanup_registry();
        return result;
    }

    result = partition_suite();
    if (result != 0) {
        CU_cleanup_registry();
        return result;
    }

    result = bubble_sort_suite();
    if (result != 0) {
        CU_cleanup_registry();
        return result;
    }

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    /* Clean up registry and return */
    CU_cleanup_registry();
    return CU_get_error();
}
