#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "random_contraction.h"

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int noop(void) { return 0; }

/*************************** min_cut ******************************************/
static void min_cut_null()
{
    int result = min_cut(NULL);
    CU_ASSERT_EQUAL(-1, result);
}

static void init_initalizes_values()
{
    /* list list; */
    /* list_item dummy; */

    /* // put some values in just make sure the test isn't passing just b/c the
     */
    /* // list just happens to be all 0 on creation */
    /* list.size = 5; */
    /* list.head = &dummy; */
    /* list.tail = &dummy; */

    /* int result = list_init(&list); */

    /* CU_ASSERT_EQUAL(0, result); */
    /* CU_ASSERT_EQUAL(0, list.size); */
    /* CU_ASSERT_PTR_NULL(list.head); */
    /* CU_ASSERT_PTR_NULL(list.tail); */
}

static int register_test_suites()
{
    CU_TestInfo init_tests[] = { CU_TEST_INFO(min_cut_null),
        CU_TEST_INFO(init_initalizes_values), CU_TEST_INFO_NULL };

    /* CU_TestInfo insert_at_tests[] = { CU_TEST_INFO(insert_null), */
    /*     CU_TEST_INFO(insert_bad_index), CU_TEST_INFO(insert_into_empty), */
    /*     CU_TEST_INFO(insert_into_tail), CU_TEST_INFO(insert_into_middle), */
    /*     CU_TEST_INFO(insert_head_creates_links), */
    /*     CU_TEST_INFO(insert_mid_creates_links), */
    /*     CU_TEST_INFO(insert_tail_creates_links), CU_TEST_INFO_NULL }; */

    /* CU_TestInfo free_tests[] */
    /*     = { CU_TEST_INFO(free_null), CU_TEST_INFO(free_calls_freeme), */
    /*           CU_TEST_INFO(free_null_freeme), CU_TEST_INFO_NULL }; */

    /* CU_TestInfo delete_at_tests[] */
    /*     = { CU_TEST_INFO(delete_null), CU_TEST_INFO(delete_calls_freeme), */
    /*           CU_TEST_INFO(delete_head_modifies_links), */
    /*           CU_TEST_INFO(delete_all_items), */
    /*           CU_TEST_INFO(delete_tail_modifies_links), */
    /*           CU_TEST_INFO(delete_mid_modifies_links), */
    /*           CU_TEST_INFO(delete_invalid_index), CU_TEST_INFO_NULL }; */

    /* CU_TestInfo get_at_tests[] = { CU_TEST_INFO(get_null), */
    /*     CU_TEST_INFO(get_retrieves_head), CU_TEST_INFO(get_retrieves_tail),
     */
    /*     CU_TEST_INFO(get_retrieves_first_half), */
    /*     CU_TEST_INFO(get_retrieves_last_half),
     * CU_TEST_INFO(get_invalid_index), */
    /*     CU_TEST_INFO_NULL }; */

    /* CU_TestInfo array_insert_tests[] = { CU_TEST_INFO(array_insert_null), */
    /*     CU_TEST_INFO(array_insert_item), CU_TEST_INFO_NULL }; */

    CU_SuiteInfo suites[] = { { .pName = "list_init suite",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = init_tests },
        /* { .pName = "list_insert_at suite", */
        /*     .pInitFunc = noop, */
        /*     .pCleanupFunc = noop, */
        /*     .pTests = insert_at_tests }, */
        /* { .pName = "list_free suite", */
        /*     .pInitFunc = free_suite_init, */
        /*     .pCleanupFunc = noop, */
        /*     .pTests = free_tests }, */
        /* { .pName = "list_delete_at suite", */
        /*     .pInitFunc = free_suite_init, */
        /*     .pCleanupFunc = noop, */
        /*     .pTests = delete_at_tests }, */
        /* { .pName = "list_get_at suite", */
        /*     .pInitFunc = noop, */
        /*     .pCleanupFunc = noop, */
        /*     .pTests = get_at_tests }, */
        /* { .pName = "array_insert suite", */
        /*     .pInitFunc = noop, */
        /*     .pCleanupFunc = noop, */
        /*     .pTests = array_insert_tests }, */
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}

int main(void)
{
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    if (register_test_suites() != 0) {
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
