#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "Dijkstra.h"
#include "include/MemAllocMock.h"
#include "include/TestHelpers.h"

typedef GraphResult (*Dijkstra)(Graph*, int);

const double EPSILON = 0.00000000000001;

const int small_n = 5;
const char* small_path = "src/graphs/small.txt";

const int med_n = 9;
const char* med_path = "src/graphs/medium.txt";

const int big_n = 201;
const char* big_path = "src/graphs/big.txt";

/************************ Helpers *********************************************/
Graph* sut;
Dijkstra _algo;

static int Distance(Vertex* v)
{
    if (v == NULL || v->data == NULL)
        return 1000000;

    ShortestPath* sp = v->data;
    return sp->distance;
}

static void NodeHasWeight(Vertex* v, double expected)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(v);
    CU_ASSERT_PTR_NOT_NULL_FATAL(v->data);

    ShortestPath* sp = v->data;
    CU_ASSERT_DOUBLE_EQUAL(expected, sp->distance, EPSILON);
}

/************************ Common **********************************************/
static void Graph_ShortestPath_null_parameter(Dijkstra algo)
{
    GraphResult result = algo(NULL, 1);
    CU_ASSERT_EQUAL(Graph_NullParameter, result);
}

static void Graph_ShortestPath_invalid_vertex(Dijkstra algo)
{
    Graph* sut = Graph_Create(5);

    GraphResult result = algo(sut, 6);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    result = Graph_DijkstraShortestPath(sut, 6);
    CU_ASSERT_EQUAL(Graph_InvalidVertexId, result);

    Graph_Destroy(sut, NULL);
}

static void Graph_ShortestPath_malloc_tester(void)
{
    GraphResult result = _algo(sut, 1);
    CU_ASSERT_EQUAL(Graph_FailedMemoryAllocation, result);
}

static void Graph_ShortestPath_failed_malloc(Dijkstra algo)
{
    sut = Graph_Create(5);
    _algo = algo;
    TestFailedMalloc(Graph_ShortestPath_malloc_tester);
    Graph_Destroy(sut, NULL);
    _algo = NULL;
}

static void Graph_ShortestPath_small(Dijkstra algo)
{
    Graph* sut = Graph_WeightedFromFile(small_n, small_path);
    GraphResult result = algo(sut, 1);

    CU_ASSERT_EQUAL(Graph_Success, result);
    NodeHasWeight(sut->V[1], 0);
    NodeHasWeight(sut->V[2], 1);
    NodeHasWeight(sut->V[3], 6);
    NodeHasWeight(sut->V[4], 3);

    Graph_Destroy(sut, free);
}

static void Graph_ShortestPath_medium(Dijkstra algo)
{
    Graph* sut = Graph_WeightedFromFile(med_n, med_path);

    GraphResult result = algo(sut, 1);

    CU_ASSERT_EQUAL(Graph_Success, result);
    NodeHasWeight(sut->V[1], 0);
    NodeHasWeight(sut->V[2], 1);
    NodeHasWeight(sut->V[3], 2);
    NodeHasWeight(sut->V[4], 3);
    NodeHasWeight(sut->V[5], 4);
    NodeHasWeight(sut->V[6], 4);
    NodeHasWeight(sut->V[7], 3);
    NodeHasWeight(sut->V[8], 2);

    Graph_Destroy(sut, free);
}

static void Graph_ShortestPath_large(Dijkstra algo)
{
    Graph* sut = Graph_WeightedFromFile(big_n, big_path);

    GraphResult result = algo(sut, 1);
    CU_ASSERT_EQUAL(Graph_Success, result);

    // Spot Checks
    CU_ASSERT_EQUAL(Distance(sut->V[7]), 2599);
    CU_ASSERT_EQUAL(Distance(sut->V[37]), 2610);
    CU_ASSERT_EQUAL(Distance(sut->V[59]), 2947);
    CU_ASSERT_EQUAL(Distance(sut->V[82]), 2052);
    CU_ASSERT_EQUAL(Distance(sut->V[99]), 2367);
    CU_ASSERT_EQUAL(Distance(sut->V[115]), 2399);
    CU_ASSERT_EQUAL(Distance(sut->V[133]), 2029);
    CU_ASSERT_EQUAL(Distance(sut->V[165]), 2442);
    CU_ASSERT_EQUAL(Distance(sut->V[188]), 2505);
    CU_ASSERT_EQUAL(Distance(sut->V[197]), 3068);

    Graph_Destroy(sut, free);
}

/************************ Naive ***********************************************/
static void Graph_DijkstraShortestPathNaive_null_parameter()
{
    Graph_ShortestPath_null_parameter(Graph_DijkstraShortestPathNaive);
}

static void Graph_DijkstraShortestPathNaive_invalid_vertex()
{
    Graph_ShortestPath_invalid_vertex(Graph_DijkstraShortestPathNaive);
}

static void Graph_DijkstraShortestPathNaive_failed_malloc()
{
    Graph_ShortestPath_failed_malloc(Graph_DijkstraShortestPathNaive);
}

static void Graph_DijkstraShortestPathNaive_small()
{
    Graph_ShortestPath_small(Graph_DijkstraShortestPathNaive);
}

static void Graph_DijkstraShortestPathNaive_medium()
{
    Graph_ShortestPath_medium(Graph_DijkstraShortestPathNaive);
}

static void Graph_DijkstraShortestPathNaive_large()
{
    Graph_ShortestPath_large(Graph_DijkstraShortestPathNaive);
}

/************************ Priority Queue **************************************/
static void Graph_DijkstraShortestPath_null_parameter()
{
    Graph_ShortestPath_null_parameter(Graph_DijkstraShortestPath);
}

static void Graph_DijkstraShortestPath_invalid_vertex()
{
    Graph_ShortestPath_invalid_vertex(Graph_DijkstraShortestPath);
}

static void Graph_DijkstraShortestPath_failed_malloc()
{
    Graph_ShortestPath_failed_malloc(Graph_DijkstraShortestPath);
}

static void Graph_DijkstraShortestPath_small()
{
    Graph_ShortestPath_small(Graph_DijkstraShortestPath);
}

static void Graph_DijkstraShortestPath_medium()
{
    Graph_ShortestPath_medium(Graph_DijkstraShortestPath);
}

static void Graph_DijkstraShortestPath_large()
{
    Graph_ShortestPath_large(Graph_DijkstraShortestPath);
}

int register_dijkstra_tests()
{
    CU_TestInfo naive_tests[]
        = { CU_TEST_INFO(Graph_DijkstraShortestPathNaive_null_parameter),
              CU_TEST_INFO(Graph_DijkstraShortestPathNaive_invalid_vertex),
              CU_TEST_INFO(Graph_DijkstraShortestPathNaive_failed_malloc),
              CU_TEST_INFO(Graph_DijkstraShortestPathNaive_small),
              CU_TEST_INFO(Graph_DijkstraShortestPathNaive_medium),
              CU_TEST_INFO(Graph_DijkstraShortestPathNaive_large),
              CU_TEST_INFO_NULL };

    CU_TestInfo tests[] = { CU_TEST_INFO(
                                Graph_DijkstraShortestPath_null_parameter),
        CU_TEST_INFO(Graph_DijkstraShortestPath_invalid_vertex),
        CU_TEST_INFO(Graph_DijkstraShortestPath_failed_malloc),
        CU_TEST_INFO(Graph_DijkstraShortestPath_small),
        CU_TEST_INFO(Graph_DijkstraShortestPath_medium),
        CU_TEST_INFO(Graph_DijkstraShortestPath_large), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Dijkstra Shortest Path - Naive",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = naive_tests },
        { .pName = "Dijkstra Shortest Path",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
