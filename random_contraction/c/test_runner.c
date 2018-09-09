#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "graph_test.h"
#include "random_contraction_test.h"

int main(void)
{
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    if (register_graph_suites() != 0 || register_min_cut_test_suites() != 0) {
        CU_cleanup_registry();
        return -1;
    }

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // You must get this value before CU_cleanup_registry() or it will
    // revert to zero
    int ret = (CU_get_number_of_failure_records() != 0);

    /* Clean up registry and return */
    CU_cleanup_registry();
    return ret;
}
