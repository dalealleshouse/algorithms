#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "quick_sort.h"

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* quick sort ****************/

void quick_sort_null()
{
    int result = quick_sort(1, 1, NULL, NULL);
    CU_ASSERT_EQUAL(-1, result);
}

int quick_sort_suite()
{
    CU_pSuite pSuite = NULL;
    pSuite = CU_add_suite("quick sort suite", init_suite, clean_suite);
    if (NULL == pSuite) {
        return -1;
    }

    /* add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "null does not throw", quick_sort_null)
            /* || NULL */
            /*     == CU_add_test( */
            /*            pSuite, "standard case", closest_distance_standard) */
            /* || NULL */
            /*     == CU_add_test( */
            /*            pSuite, "standard case 2", closest_distance_standard2)
             */
            /* || NULL */
            /*     == CU_add_test(pSuite, "large random array matches slow
               result", */
            /*            closest_distance_matches_slow) */

            )

    ) {
        return CU_get_error();
    }

    return 0;
}

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

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    /* Clean up registry and return */
    CU_cleanup_registry();
    return CU_get_error();
}
