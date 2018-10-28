#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "TestHelpers.h"

#include "Graph.h"
#include "MemAllocMock.h"

/*************************** Graph_Init ***************************************/
static void GraphIsInitalized(Graph* graph)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(0, graph->m);
    CU_ASSERT_EQUAL(0, graph->n);
    CU_ASSERT_PTR_NULL(graph->V);
}

static void Graph_Init_null_parameter()
{
    GraphResult result = Graph_Init(NULL);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);
}

static void Graph_Init_initalizes_variables()
{
    Vertex* vertex = malloc(sizeof(Vertex));
    Graph* graph = malloc(sizeof(Graph));
    graph->m = 10;
    graph->n = 10;
    graph->V = &vertex;

    GraphResult result = Graph_Init(graph);
    CU_ASSERT_EQUAL(Graph_Success, result);
    GraphIsInitalized(graph);

    Graph_Destroy(graph, NULL);
    free(vertex);
}

/*************************** Graph_Create *************************************/
static void Graph_Create_malloc_tester(void)
{
    Graph* graph = Graph_Create();
    CU_ASSERT_PTR_NULL(graph);
}

static void Graph_Create_failed_malloc()
{
    TestFaliedMalloc(Graph_Create_malloc_tester);
}

static void Graph_Create_initalizes_values()
{
    Graph* graph = Graph_Create();
    GraphIsInitalized(graph);
    Graph_Destroy(graph, NULL);
}

/*************************** Graph_VertexInit *********************************/
static void VertexIsInitalized(Vertex* vertex)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(vertex);
    CU_ASSERT_EQUAL(-1, vertex->id);
    CU_ASSERT_EQUAL(0, vertex->degree);
    CU_ASSERT_PTR_NULL(vertex->data);
    CU_ASSERT_PTR_NULL(vertex->edges);
}

static void Graph_VertexInit_null_parameter()
{
    GraphResult result = Graph_VertexInit(NULL);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);
}

static void Graph_VertexInit_initalizes_values()
{
    Vertex* v = malloc(sizeof(Vertex));

    GraphResult result = Graph_VertexInit(v);
    CU_ASSERT_EQUAL(Graph_Success, result);
    VertexIsInitalized(v);

    Graph_VertexDestroy(v, NULL);
}

/*************************** Graph_VertexCreate *******************************/
static void Graph_VertexCreate_malloc_tester(void)
{
    Vertex* vertex = Graph_VertexCreate(1, NULL);
    CU_ASSERT_PTR_NULL(vertex);
}

static void Graph_VertexCreate_failed_malloc()
{
    TestFaliedMalloc(Graph_VertexCreate_malloc_tester);
}

static void Graph_VertexCreate_intializes_values()
{
    const int id = 138;
    TestThingy* t = malloc(sizeof(TestThingy));

    Vertex* vertex = Graph_VertexCreate(id, t);
    CU_ASSERT_PTR_NOT_NULL_FATAL(vertex);
    CU_ASSERT_EQUAL(0, vertex->degree);
    CU_ASSERT_PTR_NULL(vertex->edges);
    CU_ASSERT_EQUAL(id, vertex->id);
    CU_ASSERT_PTR_EQUAL(t, vertex->data);

    Graph_VertexDestroy(vertex, NULL);
}

int register_graph_tests()
{
    CU_TestInfo Queue_tests[] = { CU_TEST_INFO(Graph_Init_null_parameter),
        CU_TEST_INFO(Graph_Init_initalizes_variables),
        CU_TEST_INFO(Graph_Create_failed_malloc),
        CU_TEST_INFO(Graph_Create_initalizes_values),
        CU_TEST_INFO(Graph_VertexInit_null_parameter),
        CU_TEST_INFO(Graph_VertexInit_initalizes_values),
        CU_TEST_INFO(Graph_VertexCreate_failed_malloc),
        CU_TEST_INFO(Graph_VertexCreate_intializes_values), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Graph",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Queue_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
