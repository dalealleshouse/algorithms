#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "MemAllocMock.h"
#include "graph.h"

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int noop(void) { return 0; }

static void intercept_malloc() { MemAllocMock_InterceptMalloc(NULL); }
static void malloc_intercepted()
{
    CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());
    MemAllocMock_ResetMalloc();
}

/*************************** Graph_Create
 * ***************************************/
static void Graph_Create_bad_malloc()
{
    intercept_malloc();

    Graph* graph = Graph_Create();
    CU_ASSERT_PTR_NULL(graph);
    malloc_intercepted();

    Graph_Destroy(graph);
}

static void Graph_Create_intializes_graph()
{
    Graph* graph = Graph_Create();

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_PTR_NULL(graph->E);
    CU_ASSERT_PTR_NULL(graph->V);
    CU_ASSERT_EQUAL(0, graph->n);
    CU_ASSERT_EQUAL(0, graph->m);
    CU_ASSERT_EQUAL(0, graph->n_allocated);

    Graph_Destroy(graph);
}

/*************************** Graph_AddVertex
 * ***************************************/
static void Graph_AddVertex_null()
{
    int result = Graph_AddVertex(NULL, 0);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);
}

static void Graph_AddVertex_bad_malloc()
{
    Graph* graph = Graph_Create();
    intercept_malloc();
    int result = Graph_AddVertex(graph, 1);

    CU_ASSERT_EQUAL(Graph_FailedMemoryAllocation, result);
    CU_ASSERT_EQUAL(0, graph->n);
    malloc_intercepted();

    Graph_Destroy(graph);
}

static void Graph_AddVertex_first()
{
    Graph* graph = Graph_Create();
    int result = Graph_AddVertex(graph, 1);

    CU_ASSERT_EQUAL(Graph_Success, result);
    CU_ASSERT_EQUAL(1, graph->n);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->V);

    CU_ASSERT_EQUAL(1, graph->V[1].vertex_id);
    CU_ASSERT_EQUAL(0, graph->V[1].degree);

    Graph_Destroy(graph);
}

static void Graph_AddVertex_adds_five()
{
    Graph* graph = Graph_Create();

    for (size_t i = 0; i < 5; i++) {
        int result = Graph_AddVertex(graph, i);
        CU_ASSERT_EQUAL(Graph_Success, result);
    }

    CU_ASSERT_EQUAL(5, graph->n);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->V);

    for (size_t i = 0; i < 5; i++) {
        CU_ASSERT_EQUAL(i, graph->V[i].vertex_id);
        CU_ASSERT_EQUAL(0, graph->V[i].degree);
    }

    Graph_Destroy(graph);
}

static void Graph_AddVertex_bad_realloc()
{
    Graph* graph = Graph_Create();

    int result = Graph_AddVertex(graph, 1);
    CU_ASSERT_EQUAL(Graph_Success, result);

    intercept_malloc();
    result = Graph_AddVertex(graph, INITIAL_ALLOC + 1);
    CU_ASSERT_EQUAL(Graph_FailedMemoryAllocation, result);
    CU_ASSERT_EQUAL(1, graph->n);
    malloc_intercepted();

    Graph_Destroy(graph);
}

static void Graph_AddVertex_pre_allocated()
{
    Graph* graph = Graph_Create();
    Graph_AddVertex(graph, 10);

    CU_ASSERT_EQUAL(10, graph->V[10].vertex_id);
    CU_ASSERT_EQUAL(true, graph->V[10].initalized);

    Graph_AddVertex(graph, 5);
    CU_ASSERT_EQUAL(5, graph->V[5].vertex_id);
    CU_ASSERT_EQUAL(true, graph->V[5].initalized);

    Graph_Destroy(graph);
}

static void Graph_AddVertex_intializes_empty_slots()
{
    Graph* graph = Graph_Create();
    Graph_AddVertex(graph, 10);

    CU_ASSERT_EQUAL(10, graph->V[10].vertex_id);
    CU_ASSERT_EQUAL(true, graph->V[10].initalized);

    for (size_t i = 0; i < graph->n_allocated; i++) {
        if (i == 10)
            continue;

        CU_ASSERT_EQUAL(false, graph->V[i].initalized);
    }

    Graph_Destroy(graph);
}

static void Graph_AddVertex_duplicate()
{
    Graph* graph = Graph_Create();
    Graph_AddVertex(graph, 10);
    int result = Graph_AddVertex(graph, 10);
    CU_ASSERT_EQUAL(Graph_DuplicateVertex, result);
    CU_ASSERT_EQUAL(1, graph->n);

    Graph_Destroy(graph);
}

/*************************** Graph_AddEdge ***********************************/
static void Graph_AddEdge_null()
{
    int result = Graph_AddEdge(NULL, 0, 0);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);
}

static void Graph_AddEdge_bad_malloc()
{
    Graph* graph = Graph_Create();
    Graph_AddVertex(graph, 1);
    Graph_AddVertex(graph, 2);

    intercept_malloc();
    int result = Graph_AddEdge(graph, 1, 2);
    CU_ASSERT_EQUAL(Graph_FailedMemoryAllocation, result);
    malloc_intercepted();

    Graph_Destroy(graph);
}

static void Graph_AddEdge_first()
{
    Graph* graph = Graph_Create();
    Graph_AddVertex(graph, 1);
    Graph_AddVertex(graph, 2);

    int result = Graph_AddEdge(graph, 1, 2);
    CU_ASSERT_EQUAL(Graph_Success, result);
    CU_ASSERT_EQUAL(1, graph->m);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->E);
    CU_ASSERT_EQUAL(1, graph->E->tail);
    CU_ASSERT_EQUAL(2, graph->E->head);
    CU_ASSERT_EQUAL(1, graph->V[1].degree);
    CU_ASSERT_EQUAL(1, graph->V[2].degree);

    Graph_Destroy(graph);
}

static void Graph_AddEdge_adds_five()
{
    Graph* graph = Graph_Create();

    for (size_t i = 0; i < 6; i++)
        Graph_AddVertex(graph, i);

    for (size_t i = 0; i < 5; i++) {
        int result = Graph_AddEdge(graph, i, i + 1);
        CU_ASSERT_EQUAL(Graph_Success, result);
    }

    CU_ASSERT_EQUAL(5, graph->m);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->E);

    for (size_t i = 0; i < 5; i++) {
        CU_ASSERT_EQUAL(i, graph->E[i].tail);
        CU_ASSERT_EQUAL(i + 1, graph->E[i].head);
    }

    for (size_t i = 0; i < 6; i++) {
        if (i == 0 || i == 5) {
            CU_ASSERT_EQUAL(1, graph->V[i].degree);
        } else
            CU_ASSERT_EQUAL(2, graph->V[i].degree);
    }

    Graph_Destroy(graph);
}

static void Graph_AddEdge_bad_realloc()
{
    Graph* graph = Graph_Create();

    for (size_t i = 0; i < INITIAL_ALLOC + 1; i++)
        Graph_AddVertex(graph, i);

    for (size_t i = 0; i < INITIAL_ALLOC; i++)
        Graph_AddEdge(graph, i, i + 1);

    intercept_malloc();
    int result = Graph_AddEdge(graph, 2, 1);
    CU_ASSERT_EQUAL(Graph_FailedMemoryAllocation, result);
    malloc_intercepted();

    Graph_Destroy(graph);
}

static void Graph_AddEdge_updates_vertex()
{
    Graph* graph = Graph_Create();
    Graph_AddVertex(graph, 1);
    Graph_AddVertex(graph, 2);
    Graph_AddVertex(graph, 3);

    Graph_AddEdge(graph, 1, 2);
    Graph_AddEdge(graph, 2, 3);
    Graph_AddEdge(graph, 2, 1);

    CU_ASSERT_EQUAL(2, graph->V[1].degree);
    CU_ASSERT_EQUAL(3, graph->V[2].degree);
    CU_ASSERT_EQUAL(1, graph->V[3].degree);

    Graph_Destroy(graph);
}

static void Graph_AddEdge_self_loop()
{
    Graph* graph = Graph_Create();
    Graph_AddVertex(graph, 1);

    int result = Graph_AddEdge(graph, 1, 1);
    CU_ASSERT_EQUAL(Graph_EdgeIsSelfLoop, result);

    Graph_Destroy(graph);
}

static void Graph_AddEdge_invalid_vertex()
{
    Graph* graph = Graph_Create();

    int result = Graph_AddEdge(graph, 1, 10);
    CU_ASSERT_EQUAL(Graph_InvalidVertex, result);

    Graph_Destroy(graph);
}

/*************************** Graph_FromFile *****************************/
static void Graph_FromFile_null()
{
    Graph* graph = Graph_FromFile(NULL);
    CU_ASSERT_PTR_NULL(graph);
    Graph_Destroy(graph);
}

static void Graph_FromFile_bad_path()
{
    Graph* graph = Graph_FromFile("some_bogus_path.txt");
    CU_ASSERT_PTR_NULL(graph);
    Graph_Destroy(graph);
}

static void Graph_FromFile_bad_malloc()
{
    intercept_malloc();
    Graph* graph = Graph_FromFile("src/graphs/Graph-4-2.txt");
    CU_ASSERT_PTR_NULL(graph);
    malloc_intercepted();

    Graph_Destroy(graph);
}

static void Graph_FromFile_small()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-4-2.txt");

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(5, graph->n);
    CU_ASSERT_EQUAL(8, graph->m);

    CU_ASSERT_EQUAL(2, graph->V[1].degree);
    CU_ASSERT_EQUAL(4, graph->V[2].degree);
    CU_ASSERT_EQUAL(3, graph->V[3].degree);
    CU_ASSERT_EQUAL(4, graph->V[4].degree);
    CU_ASSERT_EQUAL(3, graph->V[5].degree);

    Graph_Destroy(graph);
}

static void Graph_FromFile_medium()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-8-2.txt");

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(8, graph->n);
    CU_ASSERT_EQUAL(14, graph->m);

    Graph_Destroy(graph);
}

static void Graph_FromFile_standard()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-200-17.txt");

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(200, graph->n);

    Graph_Destroy(graph);
}

static void Graph_FromFile_invalid()
{
    Graph* graph = Graph_FromFile("src/graphs/BadGraph.txt");

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(1, graph->n);
    CU_ASSERT_EQUAL(0, graph->m);

    Graph_Destroy(graph);
}

/*************************** Graph_CollapseEdge ******************************/
static void Graph_CollapseEdge_null()
{
    int result = Graph_CollapseEdge(NULL, 0);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);
}

static void Graph_CollapseEdge_invalid_index()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-4-2.txt");

    int result = Graph_CollapseEdge(graph, graph->m);
    CU_ASSERT_EQUAL(Graph_InvalidEdgeIndex, result);

    Graph_Destroy(graph);
}

static void Graph_CollapseEdge_tail()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-4-2.txt");
    size_t m = graph->m;
    size_t n = graph->n;

    int result = Graph_CollapseEdge(graph, 1);

    CU_ASSERT_EQUAL(Graph_Success, result);

    CU_ASSERT_EQUAL(n - 1, graph->n);
    CU_ASSERT_EQUAL(false, graph->V[1].initalized);
    CU_ASSERT_EQUAL(true, graph->V[4].initalized);

    CU_ASSERT_EQUAL(4, graph->V[2].degree);
    CU_ASSERT_EQUAL(3, graph->V[3].degree);
    CU_ASSERT_EQUAL(4, graph->V[4].degree);
    CU_ASSERT_EQUAL(3, graph->V[5].degree);

    CU_ASSERT_EQUAL(m - 1, graph->m);

    Graph_Destroy(graph);
}

static void Graph_CollapseEdge_head()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-4-2.txt");
    size_t m = graph->m;
    size_t n = graph->n;

    int result = Graph_CollapseEdge(graph, 7);
    CU_ASSERT_EQUAL(Graph_Success, result);

    CU_ASSERT_EQUAL(n - 1, graph->n);
    CU_ASSERT_EQUAL(false, graph->V[4].initalized);
    CU_ASSERT_EQUAL(true, graph->V[5].initalized);

    CU_ASSERT_EQUAL(2, graph->V[1].degree);
    CU_ASSERT_EQUAL(4, graph->V[2].degree);
    CU_ASSERT_EQUAL(3, graph->V[3].degree);
    CU_ASSERT_EQUAL(5, graph->V[5].degree);

    CU_ASSERT_EQUAL(m - 1, graph->m);

    Graph_Destroy(graph);
}

static void Graph_CollapseEdge_self_loop()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-4-2.txt");
    size_t m = graph->m;
    size_t n = graph->n;

    // tail = 4, head = 5
    int result = Graph_CollapseEdge(graph, 7);
    CU_ASSERT_EQUAL(Graph_Success, result);

    // tail = 2, head = 5
    result = Graph_CollapseEdge(graph, 4);
    CU_ASSERT_EQUAL(Graph_Success, result);

    CU_ASSERT_EQUAL(n - 2, graph->n);

    CU_ASSERT_EQUAL(2, graph->V[1].degree);
    CU_ASSERT_EQUAL(3, graph->V[3].degree);
    CU_ASSERT_EQUAL(5, graph->V[5].degree);

    CU_ASSERT_EQUAL(m - 3, graph->m);

    Graph_Destroy(graph);
}

static void Graph_CollapseEdge_three_consecutive()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-4-2.txt");

    Graph_CollapseEdge(graph, 1);
    Graph_CollapseEdge(graph, 4);
    Graph_CollapseEdge(graph, 2);

    CU_ASSERT_EQUAL(2, graph->n);
    CU_ASSERT_EQUAL(3, graph->m);

    Graph_Destroy(graph);
}

static void Graph_CollapseEdge_large()
{
    Graph* graph = Graph_FromFile("src/graphs/Graph-200-17.txt");
    size_t m = graph->m;
    size_t n = graph->n;

    int result = Graph_CollapseEdge(graph, m - 1);
    CU_ASSERT_EQUAL(Graph_Success, result);

    CU_ASSERT_EQUAL(n - 1, graph->n);
    CU_ASSERT_EQUAL(m - 1, graph->m);

    Graph_Destroy(graph);
}

/*************************** Graph_Clone **************************************/
static void GraphAreIdentical(Graph* x, Graph* y)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(x);
    CU_ASSERT_PTR_NOT_NULL_FATAL(y);

    CU_ASSERT_PTR_NOT_EQUAL(x, y);

    CU_ASSERT_EQUAL(x->n, y->n);
    CU_ASSERT_EQUAL(x->m, y->m);
    CU_ASSERT_EQUAL(x->n_allocated, y->n_allocated);
    CU_ASSERT_EQUAL(x->m_allocated, y->m_allocated);

    if (x->V == NULL) {
        CU_ASSERT_PTR_NULL(y->V);
    } else {
        CU_ASSERT_PTR_NOT_NULL_FATAL(y->V);
        CU_ASSERT_PTR_NOT_EQUAL(x->V, y->V);
        CU_ASSERT_EQUAL(0, memcmp(x->V, y->V, sizeof(vertex) * x->n_allocated));
    }

    if (x->E == NULL) {
        CU_ASSERT_PTR_NULL(y->E);
    } else {
        CU_ASSERT_PTR_NOT_NULL_FATAL(y->E);
        CU_ASSERT_PTR_NOT_EQUAL(x->E, y->E);
        CU_ASSERT_EQUAL(0, memcmp(x->E, y->E, sizeof(edge) * x->m));
    }
}

static void Graph_Clone_null()
{
    Graph* result = Graph_Clone(NULL);
    CU_ASSERT_PTR_NULL(result);
}

static void Graph_Clone_standard()
{
    Graph* source = Graph_FromFile("src/graphs/Graph-4-2.txt");

    Graph* clone = Graph_Clone(source);
    GraphAreIdentical(source, clone);

    Graph_Destroy(source);
    Graph_Destroy(clone);
}

static void Graph_Clone_empty()
{
    Graph* source = Graph_Create();
    Graph* clone = Graph_Clone(source);

    GraphAreIdentical(source, clone);

    Graph_Destroy(source);
    Graph_Destroy(clone);
}

static void Graph_Clone_bad_malloc()
{
    Graph* source = Graph_Create();

    intercept_malloc();
    Graph* clone = Graph_Clone(source);

    CU_ASSERT_PTR_NULL(clone);
    malloc_intercepted();

    Graph_Destroy(source);
}

static void Graph_Clone_large()
{
    Graph* source = Graph_FromFile("src/graphs/Graph-200-17.txt");

    Graph* clone = Graph_Clone(source);
    GraphAreIdentical(source, clone);

    Graph_Destroy(source);
    Graph_Destroy(clone);
}

static void Graph_Clone_super_vertex()
{
    Graph* source = Graph_FromFile("src/graphs/Graph-4-2.txt");

    while (source->n > 2)
        Graph_CollapseEdge(source, 0);

    Graph* clone = Graph_Clone(source);
    GraphAreIdentical(source, clone);

    Graph_Destroy(source);
    Graph_Destroy(clone);
}

int register_graph_suites()
{
    CU_TestInfo Graph_Create_tests[] = { CU_TEST_INFO(Graph_Create_bad_malloc),
        CU_TEST_INFO(Graph_Create_intializes_graph), CU_TEST_INFO_NULL };

    CU_TestInfo Graph_AddVertex_tests[] = { CU_TEST_INFO(Graph_AddVertex_null),
        CU_TEST_INFO(Graph_AddVertex_bad_malloc),
        CU_TEST_INFO(Graph_AddVertex_first),
        CU_TEST_INFO(Graph_AddVertex_adds_five),
        CU_TEST_INFO(Graph_AddVertex_bad_realloc),
        CU_TEST_INFO(Graph_AddVertex_intializes_empty_slots),
        CU_TEST_INFO(Graph_AddVertex_pre_allocated),
        CU_TEST_INFO(Graph_AddVertex_duplicate), CU_TEST_INFO_NULL };

    CU_TestInfo Graph_AddEdge_tests[] = { CU_TEST_INFO(Graph_AddEdge_null),
        CU_TEST_INFO(Graph_AddEdge_bad_malloc),
        CU_TEST_INFO(Graph_AddEdge_first),
        CU_TEST_INFO(Graph_AddEdge_adds_five),
        CU_TEST_INFO(Graph_AddEdge_bad_realloc),
        CU_TEST_INFO(Graph_AddEdge_updates_vertex),
        CU_TEST_INFO(Graph_AddEdge_self_loop),
        CU_TEST_INFO(Graph_AddEdge_invalid_vertex), CU_TEST_INFO_NULL };

    CU_TestInfo Graph_FromFile_tests[] = { CU_TEST_INFO(Graph_FromFile_null),
        CU_TEST_INFO(Graph_FromFile_bad_path),
        CU_TEST_INFO(Graph_FromFile_bad_malloc),
        CU_TEST_INFO(Graph_FromFile_small),
        CU_TEST_INFO(Graph_FromFile_standard),
        CU_TEST_INFO(Graph_FromFile_invalid),
        CU_TEST_INFO(Graph_FromFile_medium), CU_TEST_INFO_NULL };

    CU_TestInfo Graph_CollapseEdge_tests[]
        = { CU_TEST_INFO(Graph_CollapseEdge_null),
              CU_TEST_INFO(Graph_CollapseEdge_invalid_index),
              CU_TEST_INFO(Graph_CollapseEdge_tail),
              CU_TEST_INFO(Graph_CollapseEdge_head),
              CU_TEST_INFO(Graph_CollapseEdge_self_loop),
              CU_TEST_INFO(Graph_CollapseEdge_three_consecutive),
              CU_TEST_INFO(Graph_CollapseEdge_large),
              CU_TEST_INFO_NULL };

    CU_TestInfo Graph_Clone_tests[] = { CU_TEST_INFO(Graph_Clone_null),
        CU_TEST_INFO(Graph_Clone_standard), CU_TEST_INFO(Graph_Clone_empty),
        CU_TEST_INFO(Graph_Clone_bad_malloc), CU_TEST_INFO(Graph_Clone_large),
        CU_TEST_INFO(Graph_Clone_super_vertex), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Graph_Create suite",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Graph_Create_tests },
        { .pName = "Graph_AddVertex suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Graph_AddVertex_tests },
        { .pName = "Graph_AddEdge suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Graph_AddEdge_tests },
        { .pName = "Graph_FromFile suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Graph_FromFile_tests },
        { .pName = "Graph_CollapseEdge suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Graph_CollapseEdge_tests },
        { .pName = "Graph_Clone suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Graph_Clone_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
