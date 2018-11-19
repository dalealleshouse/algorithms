#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"
#include "include/GraphSearch.h"
#include "BFS_ShortestPath.h"

/*******************************************************************************
 * unweighted.txt
 * 0
 * |
 *         1
 *       / | \
 *     2   3   4
 *    /    \         |
 *   5       6       7
 *   |     / | \
 *   8    9 10 11
 *
 * ****************************************************************************/

const static size_t small_n = 12;
const static char* small_path = "src/graphs/unweighted.txt";

static int Value(Vertex* v)
{
    if (v == NULL || v->data == NULL)
        return -1;

    return ((VertexData*)v->data)->value;
}

static void Graph_ShortestPath_standard()
{
    Graph* graph = Graph_FromFile(small_n, small_path);

    Graph_ShortestPath(graph, 4);

    CU_ASSERT_EQUAL(1, Value(graph->V[1]));
    CU_ASSERT_EQUAL(2, Value(graph->V[2]));
    CU_ASSERT_EQUAL(2, Value(graph->V[3]));
    CU_ASSERT_EQUAL(0, Value(graph->V[4]));
    CU_ASSERT_EQUAL(3, Value(graph->V[5]));
    CU_ASSERT_EQUAL(3, Value(graph->V[6]));
    CU_ASSERT_EQUAL(-1, Value(graph->V[7]));
    CU_ASSERT_EQUAL(4, Value(graph->V[8]));
    CU_ASSERT_EQUAL(4, Value(graph->V[9]));
    CU_ASSERT_EQUAL(4, Value(graph->V[10]));
    CU_ASSERT_EQUAL(4, Value(graph->V[11]));

    Graph_Destroy(graph, free);
}

int register_sp_tests()
{
    CU_TestInfo tests[] = { CU_TEST_INFO(
                                Graph_ShortestPath_standard),
        CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Shortest Path",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
