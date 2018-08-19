#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

extern int quick_sort_suite();
extern int partition_suite();
extern int bubble_sort_suite();
extern int insertion_sort_suite();
extern int selection_sort_suite();
extern int merge_sort_test_suite();

int main(void)
{
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    int result;
    if ((result = bubble_sort_suite()) != 0
        || (result = insertion_sort_suite() != 0)
        || (result = merge_sort_test_suite() != 0)
        || (result = quick_sort_suite()) != 0
        || (result = selection_sort_suite() != 0)
        || (result = partition_suite() != 0)) {
        CU_cleanup_registry();
        return -1;
    }

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // You must get this value before CU_cleanup_registry() or it will revert to
    // zero
    int ret = (CU_get_number_of_failure_records() != 0);

    /* Clean up registry and return */
    CU_cleanup_registry();
    return ret;
}
