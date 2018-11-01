#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "GraphSearch.h"
#include "TestHelpers.h"

typedef struct VertexData {
    bool am_i_conquered;
} VertexData;

static bool the_conqueror(Vertex* v)
{
    if (v->data != NULL)
        return true;

    VertexData* d = malloc(sizeof(VertexData));
    d->am_i_conquered = true;
    v->data = d;

    return true;
}

static bool is_conquered(Vertex* v)
{
    if (v->data == NULL)
        return false;

    return ((VertexData*)v->data)->am_i_conquered;
}

static SearchStrategy strategy = { the_conqueror, is_conquered };

/*************************** Breadth First Search *****************************/
static void Graph_BFS_null_parameter()
{
    Graph* graph = Graph_Create(10);

    GraphResult result = Graph_BFS(NULL, 1, &strategy);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    result = Graph_BFS(graph, 1, NULL);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    SearchStrategy bad = { NULL, is_conquered };
    result = Graph_BFS(graph, 1, &bad);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);

    SearchStrategy bad2 = { the_conqueror, NULL };
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

static void Graph_BFS_conquers()
{
    const size_t n = 12;
    Graph* graph = Graph_FromFile(n, "src/graphs/small.txt");
    Graph_BFS(graph, 7, &strategy);

    for (size_t i = 0; i < n; i++) {
        if (i == 7) {
            CU_ASSERT_PTR_NOT_NULL(graph->V[i]->data);
            CU_ASSERT_TRUE(((VertexData*)graph->V[i]->data)->am_i_conquered);
        } else
            CU_ASSERT_PTR_NULL(graph->V[i]->data);
    }

    Graph_Destroy(graph, free);
}

static void Graph_BFS_conquers_more()
{
    const size_t n = 12;
    Graph* graph = Graph_FromFile(n, "src/graphs/small.txt");
    Graph_BFS(graph, 1, &strategy);

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

int register_bfs_tests()
{
    CU_TestInfo Queue_tests[] = { CU_TEST_INFO(Graph_BFS_null_parameter),
        CU_TEST_INFO(Graph_BFS_invalid_vertex),
        CU_TEST_INFO(Graph_BFS_conquers), CU_TEST_INFO(Graph_BFS_conquers_more),
        CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Breadth First Search",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Queue_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
