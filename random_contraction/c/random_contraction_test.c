#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "random_contraction.h"

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int noop(void) { return 0; }

/*************************** RC_KargerMinCut **********************************/
static void RC_KargerMinCut_null()
{
    Graph* result = RC_KargerMinCut(NULL);
    CU_ASSERT_PTR_NULL(result);
}

static void RC_KargerMinCut_small_graph()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-4-2.txt");

    Graph* result = RC_KargerMinCut(graph);
    CU_ASSERT_EQUAL(2, result->m);

    Graph_Destroy(result);
    Graph_Destroy(graph);
}

static void RC_KargerMinCut_slightly_larger_graph()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-8-2.txt");

    Graph* result = RC_KargerMinCut(graph);
    CU_ASSERT_EQUAL(2, result->m);

    Graph_Destroy(result);
    Graph_Destroy(graph);
}

static void RC_KargerMinCut_medium_graph()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-25-6.txt");

    Graph* result = RC_KargerMinCut(graph);
    CU_ASSERT_EQUAL(6, result->m);

    Graph_Destroy(result);
    Graph_Destroy(graph);
}

static void RC_KargerMinCut_large()
{
    /* Graph* graph = Graph_FromFile("src/Graph.txt"); */

    /* int result = RC_KargerMinCut(graph); */
    /* CU_ASSERT_EQUAL(6, result); */

    /* Graph_Destroy(graph); */
}

/*************************** RC_KargerSteinMinCut *****************************/
static void RC_KargerSteinMinCut_null()
{
    Graph* result = RC_KargerSteinMinCut(NULL);
    CU_ASSERT_PTR_NULL(result);
}

static void RC_KargerSteinMinCut_small_graph()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-4-2.txt");

    Graph* result = RC_KargerSteinMinCut(graph);
    CU_ASSERT_EQUAL(2, result->m);

    Graph_Destroy(result);
    Graph_Destroy(graph);
}

static void RC_KargerSteinMinCut_slightly_larger_graph()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-8-2.txt");

    Graph* result = RC_KargerSteinMinCut(graph);
    CU_ASSERT_EQUAL(2, result->m);

    Graph_Destroy(result);
    Graph_Destroy(graph);
}

static void RC_KargerSteinMinCut_medium_graph()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-25-6.txt");

    Graph* result = RC_KargerSteinMinCut(graph);
    CU_ASSERT_EQUAL(6, result->m);

    Graph_Destroy(result);
    Graph_Destroy(graph);
}

static void RC_KargerSteinMinCut_large()
{
    /* Graph* graph = Graph_FromFile("src/Graph.txt"); */

    /* int result = RC_KargerSteinMinCut(graph); */
    /* CU_ASSERT_EQUAL(6, result); */

    /* Graph_Destroy(graph); */
}

int register_min_cut_test_suites()
{
    CU_TestInfo RC_KargerMinCut_tests[] = { CU_TEST_INFO(RC_KargerMinCut_null),
        CU_TEST_INFO(RC_KargerMinCut_small_graph),
        CU_TEST_INFO(RC_KargerMinCut_slightly_larger_graph),
        CU_TEST_INFO(RC_KargerMinCut_medium_graph),
        CU_TEST_INFO(RC_KargerMinCut_large), CU_TEST_INFO_NULL };

    CU_TestInfo RC_KargerSteinMinCut_tests[]
        = { CU_TEST_INFO(RC_KargerSteinMinCut_null),
              CU_TEST_INFO(RC_KargerSteinMinCut_small_graph),
              CU_TEST_INFO(RC_KargerSteinMinCut_slightly_larger_graph),
              CU_TEST_INFO(RC_KargerSteinMinCut_medium_graph),
              CU_TEST_INFO(RC_KargerSteinMinCut_large), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "RC_KargerMinCut suite",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = RC_KargerMinCut_tests },
        { .pName = "RC_KargenSteinMinCut suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = RC_KargerSteinMinCut_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
