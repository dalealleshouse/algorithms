#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "GraphSearch.h"
#include "TestHelpers.h"

/*******************************************************************************
 * SmallGraph.txt
 *         1
 *       / | \
 *     2   3   4
 *    /    \      |
 *   5       6    7
 *   |     / | \
 *   8    9 10 11
 * ****************************************************************************/

/*************************** Breadth First Search *****************************/
bool con(Vertex* x, Vertex* y)
{
    (void)x;
    (void)y;
    return true;
}

bool is_con(Vertex* x)
{
    (void)x;
    return true;
}

SearchStrategy strategy = { con, is_con };

static void Graph_BFS_null_parameter()
{
    Graph* graph = Graph_Create(10);

    GraphResult result = Graph_BFS(NULL, 1, &strategy);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    result = Graph_BFS(graph, 1, NULL);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    SearchStrategy bad = { NULL, is_con };
    result = Graph_BFS(graph, 1, &bad);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    SearchStrategy bad2 = { con, NULL };
    result = Graph_BFS(graph, 1, &bad2);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_BFS_invalid_vertex()
{
    Graph* graph = Graph_FromFile(12, "src/graphs/small.txt");

    GraphResult result = Graph_BFS(graph, 15, &strategy);
    CU_ASSERT_EQUAL(Graph_VertexIdExceedsMaxSize, result);

    result = Graph_BFS(graph, -1, &strategy);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    Graph_Destroy(graph, NULL);
}

/*************************** Graph_Reachable **********************************/
static void Graph_Reachable_marks_single_node_reachable()
{
    const size_t n = 12;
    Graph* graph = Graph_FromFile(n, "src/graphs/small.txt");
    Graph_Reachable(graph, 7);

    for (size_t i = 0; i < n; i++) {
        if (i == 7) {
            CU_ASSERT_PTR_NOT_NULL(graph->V[i]->data);
            CU_ASSERT_TRUE(((VertexData*)graph->V[i]->data)->am_i_conquered);
        } else
            CU_ASSERT_PTR_NULL(graph->V[i]->data);
    }

    Graph_Destroy(graph, free);
}

static void Graph_Reachable_marks_all_reachable()
{
    const size_t n = 12;
    Graph* graph = Graph_FromFile(n, "src/graphs/small.txt");
    Graph_Reachable(graph, 1);

    for (size_t i = 1; i < n; i++) {
        if (i == 7) {
            CU_ASSERT_PTR_NULL(graph->V[i]->data);
        } else {
            CU_ASSERT_PTR_NOT_NULL(graph->V[i]->data);
            CU_ASSERT_TRUE(((VertexData*)graph->V[i]->data)->am_i_conquered);
        }
    }

    Graph_Destroy(graph, free);
}

/*************************** Shortest Path ************************************/
static int ShortestPath(Vertex* v)
{
    if (v == NULL || v->data == NULL)
        return -1;

    return ((VertexData*)v->data)->shortest_path;
}

static void Graph_ShortestPath_standard()
{
    const size_t n = 12;
    Graph* graph = Graph_FromFile(n, "src/graphs/small.txt");

    Graph_ShortestPath(graph, 4);

    CU_ASSERT_EQUAL(1, ShortestPath(graph->V[1]));
    CU_ASSERT_EQUAL(2, ShortestPath(graph->V[2]));
    CU_ASSERT_EQUAL(2, ShortestPath(graph->V[3]));
    CU_ASSERT_EQUAL(0, ShortestPath(graph->V[4]));
    CU_ASSERT_EQUAL(3, ShortestPath(graph->V[5]));
    CU_ASSERT_EQUAL(3, ShortestPath(graph->V[6]));
    CU_ASSERT_EQUAL(-1, ShortestPath(graph->V[7]));
    CU_ASSERT_EQUAL(4, ShortestPath(graph->V[8]));
    CU_ASSERT_EQUAL(4, ShortestPath(graph->V[9]));
    CU_ASSERT_EQUAL(4, ShortestPath(graph->V[10]));
    CU_ASSERT_EQUAL(4, ShortestPath(graph->V[11]));

    Graph_Destroy(graph, free);
}

/*************************** Connected Components *****************************/
static int ComponentId(Vertex* v)
{
    if (v == NULL || v->data == NULL)
        return -1;

    return ((VertexData*)v->data)->component_id;
}

static void Graph_Connected_standard()
{
    const size_t n = 12;
    Graph* graph = Graph_FromFile(n, "src/graphs/small.txt");

    Graph_Connected(graph);

    CU_ASSERT_EQUAL(1, ComponentId(graph->V[0]));
    CU_ASSERT_EQUAL(2, ComponentId(graph->V[1]));
    CU_ASSERT_EQUAL(2, ComponentId(graph->V[2]));
    CU_ASSERT_EQUAL(2, ComponentId(graph->V[3]));
    CU_ASSERT_EQUAL(2, ComponentId(graph->V[4]));
    CU_ASSERT_EQUAL(2, ComponentId(graph->V[5]));
    CU_ASSERT_EQUAL(2, ComponentId(graph->V[6]));
    CU_ASSERT_EQUAL(3, ComponentId(graph->V[7]));
    CU_ASSERT_EQUAL(2, ComponentId(graph->V[8]));
    CU_ASSERT_EQUAL(2, ComponentId(graph->V[9]));
    CU_ASSERT_EQUAL(2, ComponentId(graph->V[10]));
    CU_ASSERT_EQUAL(2, ComponentId(graph->V[11]));

    Graph_Destroy(graph, free);
}

int register_bfs_tests()
{
    CU_TestInfo BFS_tests[] = { CU_TEST_INFO(Graph_BFS_null_parameter),
        CU_TEST_INFO(Graph_BFS_invalid_vertex), CU_TEST_INFO_NULL };

    CU_TestInfo Reachable_Tests[]
        = { CU_TEST_INFO(Graph_Reachable_marks_single_node_reachable),
              CU_TEST_INFO(Graph_Reachable_marks_all_reachable),
              CU_TEST_INFO_NULL };

    CU_TestInfo ShortestPath_Tests[]
        = { CU_TEST_INFO(Graph_ShortestPath_standard), CU_TEST_INFO_NULL };

    CU_TestInfo Connected_Tests[]
        = { CU_TEST_INFO(Graph_Connected_standard), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Breadth First Search",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = BFS_tests },
        { .pName = "Reachable",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Reachable_Tests },
        { .pName = "Shortest Path",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = ShortestPath_Tests },
        { .pName = "Connected",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Connected_Tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
