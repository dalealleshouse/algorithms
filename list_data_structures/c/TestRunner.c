#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

int register_binary_tree_tests();
int register_linked_list_tests();
int register_array_tests();

int main(void)
{
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    if (register_binary_tree_tests() != 0 || register_linked_list_tests() != 0
        || register_array_tests() != 0) {
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
