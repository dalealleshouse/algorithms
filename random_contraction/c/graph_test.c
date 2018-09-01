
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "graph.h"

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int noop(void) { return 0; }

/*************************** graph ********************************************/

// Create graph
// Insert vertex
// Insert edge
// delete vertex
// delete edge

/*************************** min_cut ******************************************/
static void graph_init_test()
{
    int result = graph_init();
    CU_ASSERT_EQUAL(-1, result);
}

int register_graph_suites()
{
    CU_TestInfo graph_tests[]
        = { CU_TEST_INFO(graph_init_test), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "graph suite",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = graph_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
