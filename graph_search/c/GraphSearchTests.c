#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "GraphSearch.h"
#include "TestHelpers.h"

/*******************************************************************************
 * small.txt
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
 * -----------------------------------------------------------------------------
 *
 * directed.txt
 *    1
 *   * \
 *  /   *
 * 0     3
 *  \   *
 *   * /
 *    2
 * ****************************************************************************/

const static size_t small_n = 12;
const static size_t dir_n = 4;
const static size_t scc_n = 12;
const static size_t simple_n = 10;
const static char* small_path = "src/graphs/small.txt";
const static char* dir_path = "src/graphs/directed.txt";
const static char* scc_path = "src/graphs/small-scc.txt";
const static char* simple_scc_path = "src/graphs/simple-scc.txt";

Graph* CreateSut() { return Graph_FromFile(small_n, small_path); }
Graph* CreateDirSut() { return Graph_FromFile(dir_n, dir_path); }
Graph* CreateSCCSut() { return Graph_FromFile(scc_n, scc_path); }
Graph* CreateSimpleSCCSut()
{
    return Graph_FromFile(simple_n, simple_scc_path);
}

static int Value(Vertex* v)
{
    if (v == NULL || v->data == NULL)
        return -1;

    return ((VertexData*)v->data)->value;
}

typedef struct Conqured {
    bool is_conquered;
} Conqured;

// returns false if the current vertex id is 5, this is useful for ensuring that
// the search algorithms stop when the conqueror return false
bool conquer(Vertex* x, Vertex* y)
{
    (void)y;
    // if null, or already conquered, return false
    if (x == NULL || x->data != NULL)
        return false;

    // mark as conquered
    Conqured* c = malloc(sizeof(c));
    c->is_conquered = true;
    x->data = c;

    if (x->id == 5)
        return false;

    return true;
}

bool is_con(Vertex* v)
{
    if (v->data == NULL)
        return false;

    return ((Conqured*)v->data)->is_conquered;
}

SearchStrategy strategy = { conquer, is_con };

/*************************** Breadth First Search *****************************/
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

    SearchStrategy bad2 = { conquer, NULL };
    result = Graph_BFS(graph, 1, &bad2);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_BFS_invalid_vertex()
{
    Graph* graph = CreateSut();

    GraphResult result = Graph_BFS(graph, 15, &strategy);
    CU_ASSERT_EQUAL(Graph_VertexIdExceedsMaxSize, result);

    result = Graph_BFS(graph, -1, &strategy);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_BFS_stops_on_false_con()
{
    Graph* graph = CreateSut();

    Graph_BFS(graph, 1, &strategy);

    for (size_t i = 0; i < small_n; i++) {
        if (i > 0 && i <= 6) {
            CU_ASSERT_TRUE(is_con(graph->V[i]));
        } else {
            CU_ASSERT_FALSE(is_con(graph->V[i]));
        }
    }

    Graph_Destroy(graph, free);
}

/*************************** Graph_Reachable **********************************/
static void Graph_Reachable_marks_single_node_reachable()
{
    Graph* graph = CreateSut();
    Graph_Reachable(graph, 7);

    for (size_t i = 0; i < small_n; i++) {
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
    Graph* graph = CreateSut();
    Graph_Reachable(graph, 1);

    for (size_t i = 1; i < small_n; i++) {
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
static void Graph_ShortestPath_standard()
{
    Graph* graph = CreateSut();

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

/*************************** Connected Components *****************************/
static void Graph_Connected_standard()
{
    Graph* graph = CreateSut();

    Graph_Connected(graph);

    CU_ASSERT_EQUAL(1, Value(graph->V[0]));
    CU_ASSERT_EQUAL(2, Value(graph->V[1]));
    CU_ASSERT_EQUAL(2, Value(graph->V[2]));
    CU_ASSERT_EQUAL(2, Value(graph->V[3]));
    CU_ASSERT_EQUAL(2, Value(graph->V[4]));
    CU_ASSERT_EQUAL(2, Value(graph->V[5]));
    CU_ASSERT_EQUAL(2, Value(graph->V[6]));
    CU_ASSERT_EQUAL(3, Value(graph->V[7]));
    CU_ASSERT_EQUAL(2, Value(graph->V[8]));
    CU_ASSERT_EQUAL(2, Value(graph->V[9]));
    CU_ASSERT_EQUAL(2, Value(graph->V[10]));
    CU_ASSERT_EQUAL(2, Value(graph->V[11]));

    Graph_Destroy(graph, free);
}

/*************************** Depth First Search *******************************/
static void Graph_DFS_null_parameter()
{
    Graph* graph = Graph_Create(10);

    GraphResult result = Graph_DFS(NULL, 1, &strategy);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    result = Graph_DFS(graph, 1, NULL);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    SearchStrategy bad = { NULL, is_con };
    result = Graph_DFS(graph, 1, &bad);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    SearchStrategy bad2 = { conquer, NULL };
    result = Graph_DFS(graph, 1, &bad2);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_DFS_invalid_vertex()
{
    Graph* graph = CreateSut();

    GraphResult result = Graph_DFS(graph, 15, &strategy);
    CU_ASSERT_EQUAL(Graph_VertexIdExceedsMaxSize, result);

    result = Graph_DFS(graph, -1, &strategy);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_DFS_stops_on_false_con()
{
    Graph* graph = CreateSut();

    Graph_DFS(graph, 1, &strategy);

    for (size_t i = 0; i < small_n; i++) {
        if (i == 1 || i == 2 || i == 5) {
            CU_ASSERT_TRUE(is_con(graph->V[i]));
        } else {
            CU_ASSERT_FALSE(is_con(graph->V[i]));
        }
    }

    Graph_Destroy(graph, free);
}

/*************************** Topological Sort *********************************/
static void Graph_TopSort_null_paramter()
{
    GraphResult result = Graph_TopSort(NULL);

    CU_ASSERT_EQUAL(Graph_NullParameter, result);
}

static void Graph_TopSort_standard()
{
    Graph* graph = CreateDirSut();

    GraphResult result = Graph_TopSort(graph);

    CU_ASSERT_EQUAL(Graph_Success, result);
    CU_ASSERT_EQUAL(1, Value(graph->V[0]));
    CU_ASSERT_EQUAL(2, Value(graph->V[1]));
    CU_ASSERT_EQUAL(3, Value(graph->V[2]));
    CU_ASSERT_EQUAL(4, Value(graph->V[3]));

    Graph_Destroy(graph, free);
}

/*************************** Strongly Connected Components ********************/
static void Graph_SCCOrder_null_paramter()
{
    int* result = Graph_SCCOrder(NULL);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(Graph_NullParameter, ErrorReporter_LastErrorCode());
}

static void Graph_SCCOrder_standard()
{
    int expected[] = { 1, 7, 4, 9, 6, 3, 8, 2, 5, 0 };
    Graph* graph = CreateSimpleSCCSut();

    int* result = Graph_SCCOrder(graph);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    for (int i = 0; i < (int)simple_n; i++)
        CU_ASSERT_EQUAL(expected[i], result[i]);

    Graph_Destroy(graph, free);
    free(result);
}

static void Graph_SCCOrder_standard_two()
{
    int expected[] = { 7, 10, 11, 4, 1, 5, 3, 2, 9, 6, 8, 0 };
    Graph* graph = CreateSCCSut();

    int* result = Graph_SCCOrder(graph);

    CU_ASSERT_PTR_NOT_NULL_FATAL(result);
    for (int i = 0; i < (int)scc_n; i++)
        CU_ASSERT_EQUAL(expected[i], result[i]);

    Graph_Destroy(graph, free);
    free(result);
}

static void Graph_SCC_null_parameter()
{
    GraphResult result = Graph_SCC(NULL);

    CU_ASSERT_EQUAL(Graph_NullParameter, result);
}

static void Graph_SCC_standard()
{
    Graph* graph = CreateSCCSut();

    GraphResult result = Graph_SCC(graph);

    CU_ASSERT_EQUAL(Graph_Success, result);
    /* CU_ASSERT_EQUAL(1, Value(graph->V[6])); */
    /* CU_ASSERT_EQUAL(1, Value(graph->V[9])); */
    /* CU_ASSERT_EQUAL(1, Value(graph->V[10])); */

    /* CU_ASSERT_EQUAL(2, Value(graph->V[3])); */

    /* CU_ASSERT_EQUAL(3, Value(graph->V[0])); */
    /* CU_ASSERT_EQUAL(3, Value(graph->V[1])); */
    /* CU_ASSERT_EQUAL(3, Value(graph->V[2])); */
    /* CU_ASSERT_EQUAL(3, Value(graph->V[4])); */

    /* CU_ASSERT_EQUAL(4, Value(graph->V[5])); */
    /* CU_ASSERT_EQUAL(4, Value(graph->V[7])); */
    /* CU_ASSERT_EQUAL(4, Value(graph->V[8])); */

    Graph_Destroy(graph, free);
}

int register_graph_search_tests()
{
    CU_TestInfo BFS_tests[] = { CU_TEST_INFO(Graph_BFS_null_parameter),
        CU_TEST_INFO(Graph_BFS_invalid_vertex),
        CU_TEST_INFO(Graph_BFS_stops_on_false_con), CU_TEST_INFO_NULL };

    CU_TestInfo Reachable_Tests[]
        = { CU_TEST_INFO(Graph_Reachable_marks_single_node_reachable),
              CU_TEST_INFO(Graph_Reachable_marks_all_reachable),
              CU_TEST_INFO_NULL };

    CU_TestInfo ShortestPath_Tests[]
        = { CU_TEST_INFO(Graph_ShortestPath_standard), CU_TEST_INFO_NULL };

    CU_TestInfo Connected_Tests[]
        = { CU_TEST_INFO(Graph_Connected_standard), CU_TEST_INFO_NULL };

    CU_TestInfo DFS_tests[] = { CU_TEST_INFO(Graph_DFS_invalid_vertex),
        CU_TEST_INFO(Graph_DFS_null_parameter),
        CU_TEST_INFO(Graph_DFS_stops_on_false_con), CU_TEST_INFO_NULL };

    CU_TestInfo Top_Tests[] = { CU_TEST_INFO(Graph_TopSort_null_paramter),
        CU_TEST_INFO(Graph_TopSort_standard), CU_TEST_INFO_NULL };

    CU_TestInfo SCC_tests[] = { CU_TEST_INFO(Graph_SCCOrder_null_paramter),
        CU_TEST_INFO(Graph_SCCOrder_standard),
        CU_TEST_INFO(Graph_SCCOrder_standard_two),
        CU_TEST_INFO(Graph_SCC_null_parameter),
        CU_TEST_INFO(Graph_SCC_standard), CU_TEST_INFO_NULL };

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
        { .pName = "Depth First Search",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = DFS_tests },
        { .pName = "Topological Sort",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Top_Tests },
        { .pName = "Strongly Connected Components",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = SCC_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
