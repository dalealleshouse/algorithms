#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "TestHelpers.h"

#include "ErrorReporter.h"
#include "Graph.h"
#include "MemAllocMock.h"

static Graph* _graph;

/*************************** Graph_Create *************************************/
static void Graph_Create_malloc_tester(void)
{
    Graph* graph = Graph_Create(1);
    CU_ASSERT_PTR_NULL(graph);
    CU_ASSERT_EQUAL(
        Graph_FailedMemoryAllocation, ErrorReporter_LastErrorCode());
}

static void Graph_Create_failed_malloc()
{
    TestFailedMalloc(Graph_Create_malloc_tester);
}

static void Graph_Create_initalizes_values()
{
    size_t n = 10;
    Graph* graph = Graph_Create(n);

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(0, graph->m);
    CU_ASSERT_EQUAL(n, graph->n);

    CU_ASSERT_PTR_NOT_NULL(graph->V);
    for (size_t i = 0; i < n; i++)
        CU_ASSERT_PTR_NOT_NULL(graph->V[i]);

    Graph_Destroy(graph, NULL);
}

/*************************** Graph_AddEdge ************************************/
static void Graph_AddEdge_malloc_tester(void)
{
    GraphResult result = Graph_AddEdge(_graph, 1, 2);
    CU_ASSERT_EQUAL(Graph_FailedMemoryAllocation, result);
}

static void Graph_AddEdge_failed_malloc()
{
    _graph = Graph_Create(10);
    TestFailedMalloc(Graph_AddEdge_malloc_tester);
    Graph_Destroy(_graph, NULL);
}

static void Graph_AddEdge_null_parameter()
{
    GraphResult result = Graph_AddEdge(NULL, 1, 2);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);
}

static void Graph_AddEdge_invalid_head()
{
    Graph* graph = Graph_Create(10);

    // Negative vertex ID
    GraphResult result = Graph_AddEdge(graph, -1, 1);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    result = Graph_AddEdge(graph, 10, 1);
    CU_ASSERT_EQUAL(Graph_VertexIdExceedsMaxSize, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_AddEdge_invalid_tail()
{
    Graph* graph = Graph_Create(10);

    // Negative vertex ID
    GraphResult result = Graph_AddEdge(graph, 1, -1);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    result = Graph_AddEdge(graph, 1, 10);
    CU_ASSERT_EQUAL(Graph_VertexIdExceedsMaxSize, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_AddEdge_intializes_values()
{
    Graph* graph = Graph_Create(10);

    GraphResult result = Graph_AddEdge(graph, 1, 2);

    CU_ASSERT_EQUAL(Graph_Success, result);
    CU_ASSERT_PTR_NOT_NULL(graph->V[2]->edges);
    CU_ASSERT_EQUAL(1, graph->V[2]->edges->head);
    CU_ASSERT_EQUAL(1, graph->m);

    Graph_Destroy(graph, NULL);
}

static void Graph_AddEdge_sets_incoming()
{
    Graph* graph = Graph_Create(10);

    GraphResult result = Graph_AddEdge(graph, 1, 2);

    CU_ASSERT_EQUAL(Graph_Success, result);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->V[1]->in_edges);
    CU_ASSERT_EQUAL(2, graph->V[1]->in_edges->tail);

    Graph_Destroy(graph, NULL);
}

/*************************** Graph_FromFile ***********************************/
static void Graph_FromFile_null_parameter()
{
    Graph* graph = Graph_FromFile(10, NULL);
    CU_ASSERT_EQUAL(Graph_NullParameter, ErrorReporter_LastErrorCode());
    CU_ASSERT_PTR_NULL(graph);
}

static void Graph_FromFile_malloc_tester(void)
{
    Graph* graph = Graph_FromFile(6, "src/graphs/Graph-4-2.txt");
    CU_ASSERT_EQUAL(
        Graph_FailedMemoryAllocation, ErrorReporter_LastErrorCode());
    CU_ASSERT_PTR_NULL(graph);
}

static void Graph_FromFile_failed_malloc()
{
    TestFailedMalloc(Graph_FromFile_malloc_tester);
}

static void Graph_FromFile_invalid_path()
{
    Graph* graph = Graph_FromFile(10, "bad_path.txt");
    CU_ASSERT_EQUAL(Graph_InvalidFilePath, ErrorReporter_LastErrorCode());
    CU_ASSERT_PTR_NULL(graph);
}

static void Graph_FromFile_insufficent_size()
{
    Graph* graph = Graph_FromFile(5, "src/graphs/Graph-4-2.txt");
    CU_ASSERT_PTR_NOT_NULL(graph);
    CU_ASSERT_EQUAL(
        Graph_VertexIdExceedsMaxSize, ErrorReporter_LastErrorCode());

    Graph_Destroy(graph, NULL);
}

static void Graph_FromFile_bad_data()
{
    Graph* graph = Graph_FromFile(3, "src/graphs/BadGraph.txt");
    CU_ASSERT_PTR_NOT_NULL(graph);
    Graph_Destroy(graph, NULL);
}

static void Graph_FromFile_standard()
{
    Graph* graph = Graph_FromFile(9, "src/graphs/Graph-4-2.txt");

    CU_ASSERT_PTR_NOT_NULL(graph);
    CU_ASSERT_EQUAL(9, graph->n);
    CU_ASSERT_EQUAL(10, graph->m);

    Graph_Destroy(graph, NULL);
}

/*************************** Graph_Destroy ************************************/
static void Graph_Destroy_complex_graph()
{
    const size_t n = 10;
    Graph* graph = Graph_Create(n);
    for (size_t i = 0; i < n; i++) {
        TestThingy* thing = malloc(sizeof(TestThingy));
        graph->V[i]->data = thing;
    }

    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (j == i)
                continue;

            GraphResult result = Graph_AddEdge(graph, i, j);
            CU_ASSERT_EQUAL(Graph_Success, result);
        }
    }

    CU_ASSERT_EQUAL(n, graph->n);
    CU_ASSERT_EQUAL(90, graph->m);

    Graph_Destroy(graph, free);
}

int register_graph_tests()
{
    CU_TestInfo Queue_tests[] = { CU_TEST_INFO(Graph_Create_failed_malloc),
        CU_TEST_INFO(Graph_Create_initalizes_values),
        CU_TEST_INFO(Graph_AddEdge_failed_malloc),
        CU_TEST_INFO(Graph_AddEdge_null_parameter),
        CU_TEST_INFO(Graph_AddEdge_invalid_head),
        CU_TEST_INFO(Graph_AddEdge_invalid_tail),
        CU_TEST_INFO(Graph_AddEdge_intializes_values),
        CU_TEST_INFO(Graph_AddEdge_sets_incoming),
        CU_TEST_INFO(Graph_FromFile_failed_malloc),
        CU_TEST_INFO(Graph_FromFile_null_parameter),
        CU_TEST_INFO(Graph_FromFile_invalid_path),
        CU_TEST_INFO(Graph_FromFile_insufficent_size),
        CU_TEST_INFO(Graph_FromFile_bad_data),
        CU_TEST_INFO(Graph_FromFile_standard),
        CU_TEST_INFO(Graph_Destroy_complex_graph), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Graph",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Queue_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
