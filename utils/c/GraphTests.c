#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "TestHelpers.h"

#include "Graph.h"
#include "MemAllocMock.h"

static Graph* _graph;

/*************************** Graph_Create *************************************/
static void Graph_Create_malloc_tester(void)
{
    Graph* graph = Graph_Create(1);
    CU_ASSERT_PTR_NULL(graph);
}

static void Graph_Create_failed_malloc()
{
    TestFaliedMalloc(Graph_Create_malloc_tester);
}

static void Graph_Create_initalizes_values()
{
    size_t max_size = 10;
    Graph* graph = Graph_Create(max_size);

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(0, graph->m);
    CU_ASSERT_EQUAL(0, graph->n);
    CU_ASSERT_EQUAL(max_size, graph->max_size);

    CU_ASSERT_PTR_NOT_NULL(graph->V);
    for (size_t i = 0; i < max_size; i++)
        CU_ASSERT_PTR_NULL(graph->V[i]);

    Graph_Destroy(graph, NULL);
}

/*************************** Graph_AddVertex **********************************/
static void Graph_AddVertex_malloc_tester(void)
{
    GraphResult result = Graph_AddVertex(_graph, 1, NULL);
    CU_ASSERT_EQUAL(Graph_FailedMemoryAllocation, result);
}

static void Graph_AddVertex_failed_malloc()
{
    _graph = Graph_Create(10);
    TestFaliedMalloc(Graph_AddVertex_malloc_tester);
    Graph_Destroy(_graph, NULL);
}

static void Graph_AddVertex_null_parameter()
{

    GraphResult result = Graph_AddVertex(NULL, -1, NULL);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);
}

static void Graph_AddVertex_negative_id()
{
    Graph* graph = Graph_Create(1);

    GraphResult result = Graph_AddVertex(graph, -1, NULL);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_AddVertex_duplicate_vertex()
{
    const int id = 5;
    Graph* graph = Graph_Create(id + 1);

    GraphResult result = Graph_AddVertex(graph, id, NULL);
    CU_ASSERT_EQUAL(Graph_Success, result);

    result = Graph_AddVertex(graph, id, NULL);
    CU_ASSERT_EQUAL(Graph_DuplicateVertexId, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_AddVertex_exceeds_maximum()
{
    Graph* graph = Graph_Create(1);

    GraphResult result = Graph_AddVertex(graph, 5, NULL);
    CU_ASSERT_EQUAL(Graph_VertexIdExceedsMaxSize, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_AddVertex_adds_vertex()
{
    const int id = 1;
    TestThingy thingy;
    Graph* graph = Graph_Create(5);

    GraphResult result = Graph_AddVertex(graph, id, &thingy);

    CU_ASSERT_EQUAL(Graph_Success, result);
    CU_ASSERT_EQUAL(1, graph->n);
    CU_ASSERT_EQUAL(0, graph->V[id]->degree);
    CU_ASSERT_PTR_NULL(graph->V[id]->edges);
    CU_ASSERT_PTR_EQUAL(&thingy, graph->V[id]->data);

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
    Graph_AddVertex(_graph, 1, NULL);
    Graph_AddVertex(_graph, 2, NULL);
    TestFaliedMalloc(Graph_AddEdge_malloc_tester);
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
    Graph_AddVertex(graph, 1, NULL);

    // Negative vertex ID
    GraphResult result = Graph_AddEdge(graph, -1, 1);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    result = Graph_AddEdge(graph, 5, 1);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    result = Graph_AddEdge(graph, 10, 1);
    CU_ASSERT_EQUAL(Graph_VertexIdExceedsMaxSize, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_AddEdge_invalid_tail()
{
    Graph* graph = Graph_Create(10);
    Graph_AddVertex(graph, 1, NULL);

    // Negative vertex ID
    GraphResult result = Graph_AddEdge(graph, 1, -1);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    result = Graph_AddEdge(graph, 1, 5);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    result = Graph_AddEdge(graph, 1, 10);
    CU_ASSERT_EQUAL(Graph_VertexIdExceedsMaxSize, result);

    Graph_Destroy(graph, NULL);
}

static void Graph_AddEdge_intializes_values()
{
    Graph* graph = Graph_Create(10);
    Graph_AddVertex(graph, 1, NULL);
    Graph_AddVertex(graph, 2, NULL);

    GraphResult result = Graph_AddEdge(graph, 1, 2);

    CU_ASSERT_EQUAL(Graph_Success, result);
    CU_ASSERT_PTR_NOT_NULL(graph->V[2]->edges);
    CU_ASSERT_EQUAL(1, graph->V[2]->edges->head);
    CU_ASSERT_EQUAL(1, graph->m);

    Graph_Destroy(graph, NULL);
}

/*************************** Graph_Destroy ************************************/
static void Graph_Destroy_complex_graph()
{
    const size_t n = 10;
    Graph* graph = Graph_Create(n);
    for (size_t i = 0; i < n; i++) {
        TestThingy* thing = malloc(sizeof(TestThingy));
        Graph_AddVertex(graph, i, thing);
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
        CU_TEST_INFO(Graph_AddVertex_negative_id),
        CU_TEST_INFO(Graph_AddVertex_null_parameter),
        CU_TEST_INFO(Graph_AddVertex_duplicate_vertex),
        CU_TEST_INFO(Graph_AddVertex_exceeds_maximum),
        CU_TEST_INFO(Graph_AddVertex_failed_malloc),
        CU_TEST_INFO(Graph_AddVertex_adds_vertex),
        CU_TEST_INFO(Graph_AddEdge_failed_malloc),
        CU_TEST_INFO(Graph_AddEdge_null_parameter),
        CU_TEST_INFO(Graph_AddEdge_invalid_head),
        CU_TEST_INFO(Graph_AddEdge_invalid_tail),
        CU_TEST_INFO(Graph_AddEdge_intializes_values),
        CU_TEST_INFO(Graph_Destroy_complex_graph), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Graph",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Queue_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
