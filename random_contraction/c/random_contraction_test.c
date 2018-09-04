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

static void min_cut_small_graph()
{
    graph* graph = graph_read_from_file("src/SmallGraph.txt");

    int result = min_cut(graph);
    CU_ASSERT_EQUAL(2, result);

    graph_destroy(graph);
}

static void min_cut_standard()
{
    graph* graph = graph_read_from_file("src/GraphMedium.txt");

    int result = min_cut(graph);
    CU_ASSERT_EQUAL(2, result);

    graph_destroy(graph);
}

static void min_cut_large()
{
    graph* graph = graph_read_from_file("src/input_random_10_25.txt");

    int result = min_cut(graph);
    CU_ASSERT_EQUAL(10, result);

    graph_destroy(graph);
}

int register_min_cut_test_suites()
{
    CU_TestInfo min_cut_tests[] = { CU_TEST_INFO(min_cut_null),
        CU_TEST_INFO(min_cut_small_graph), CU_TEST_INFO(min_cut_standard),
        CU_TEST_INFO(min_cut_large), CU_TEST_INFO_NULL };

    CU_TestInfo collapse_edge_tests[] = { CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "min_cut suite",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = min_cut_tests },
        { .pName = "collapse_edge suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = collapse_edge_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
