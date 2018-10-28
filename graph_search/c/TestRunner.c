#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "Graph.h"
#include "MemAllocMock.h"

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int noop(void) { return 0; }

typedef struct TestThingy {
    int id;
    size_t n;
    int x;
    int y;
    double z;
} TestThingy;

/*************************** some_function ************************************/
static void place_holder()
{
    CU_PASS();
    /* MemAllocMock_InterceptMalloc(NULL); */
    /* Queue* sut = Queue_Create(NULL); */
    /* CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount()); */
    /* CU_ASSERT_PTR_NULL(sut); */
    /* MemAllocMock_ResetMalloc(); */
}

static int register_tests()
{
    CU_TestInfo Queue_tests[]
        = { CU_TEST_INFO(place_holder), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Queue",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Queue_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}

int main(void)
{
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    if (register_tests() != 0) {
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
