#include <limits.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "MinSpanningTree.h"
#include "include/Graph.h"
#include "include/ResultCode.h"
#include "include/TestHelpers.h"

Graph* SimpleGraph()
{
    return Graph_WeightedFromFile(5, "src/data/simple.txt");
}

typedef Result (*min_span_tree)(const Graph* graph, MinSpanTree* mst);

// Common Test Functions
static void NullParameterTest(min_span_tree minSpanTree)
{
    Graph* input = SimpleGraph();

    // Both parameters NULL
    Result result = minSpanTree(NULL, NULL);
    CU_ASSERT_EQUAL(result, NullParameter);

    // Result is null
    result = minSpanTree(input, NULL);
    CU_ASSERT_EQUAL(result, NullParameter);

    // Input is NULL
    MinSpanTree mst;
    result = minSpanTree(NULL, &mst);
    CU_ASSERT_EQUAL(result, NullParameter);

    Graph_Destroy(input, free);
}

static void FailedMallocTest(min_span_tree minSpanTree)
{
    Graph* input = SimpleGraph();
    Result result;

    MinSpanTree mst;

    FAILED_MALLOC_TEST({ result = minSpanTree(input, &mst); });
    CU_ASSERT_EQUAL(result, FailedMemoryAllocation);

    Graph_Destroy(input, free);
}

static void _validateSimpleGraphResults(const MinSpanTree* mst)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(mst);
    CU_ASSERT_EQUAL(mst->cost, 7);
    CU_ASSERT_EQUAL(mst->edge_count, 3);
    Edge* edge = mst->edge_head;

    // first edge
    CU_ASSERT_PTR_NOT_NULL_FATAL(edge);
    CU_ASSERT_EQUAL(edge->weight, 1);

    // second edge
    edge = edge->next;
    CU_ASSERT_PTR_NOT_NULL_FATAL(edge);
    CU_ASSERT_EQUAL(edge->weight, 2);

    // third edge
    edge = edge->next;
    CU_ASSERT_PTR_NOT_NULL_FATAL(edge);
    CU_ASSERT_EQUAL(edge->weight, 4);

    CU_ASSERT_PTR_NULL(edge->next);
}

static void CalculatesMstTest(min_span_tree minSpanTree)
{
    MinSpanTree mst;
    Graph* input = SimpleGraph();

    Result result = minSpanTree(input, &mst);
    Graph_Destroy(input, free);

    CU_ASSERT_EQUAL(result, Success);
    _validateSimpleGraphResults(&mst);

    MinSpanTree_Edges_Destroy(mst.edge_head);
}

static void ClearsInitialMstValuesTest(min_span_tree minSpanTree)
{
    MinSpanTree mst;
    mst.cost = 5;
    Edge* edge = malloc(sizeof(Edge));
    edge->next = NULL;
    mst.edge_head = edge;

    Graph* input = SimpleGraph();

    Result result = minSpanTree(input, &mst);
    Graph_Destroy(input, free);
    free(edge);

    CU_ASSERT_EQUAL(result, Success);
    _validateSimpleGraphResults(&mst);

    MinSpanTree_Edges_Destroy(mst.edge_head);
}

static void ArithmeticOverflowTest(min_span_tree minSpanTree)
{
    Graph* input = SimpleGraph();

    for (size_t i = 1; i < input->n; i++) {
        Edge* e = input->V[i]->edges;
        while (e != NULL) {
            e->weight = INT_MAX;
            e = e->next;
        }
    }

    MinSpanTree mst
        = { .cost = 0, .edge_count = 0, .edge_head = NULL, .edge_tail = NULL };
    Result result = minSpanTree(input, &mst);
    CU_ASSERT_EQUAL(result, ArithmeticOverflow);

    Graph_Destroy(input, free);
    MinSpanTree_Edges_Destroy(mst.edge_head);
}

// PrimMinSpanTreeNaive

static void PrimMinSpanTreeNaive_NullParameter()
{
    NullParameterTest(PrimMinSpanTreeNaive);
}

static void PrimMinSpanTreeNaive_FailedMalloc()
{
    FailedMallocTest(PrimMinSpanTreeNaive);
}

static void PrimMinSpanTreeNaive_CalculatesMst()
{
    CalculatesMstTest(PrimMinSpanTreeNaive);
}

static void PrimMinSpanTreeNaive_ClearsInitalMstValues()
{
    ClearsInitialMstValuesTest(PrimMinSpanTreeNaive);
}

static void PrimMinSpanTreeNaive_ArithmeticOverflow()
{
    ArithmeticOverflowTest(PrimMinSpanTreeNaive);
}

// PrimMinSpanTree
static void PrimMinSpanTree_NullParameter()
{
    NullParameterTest(PrimMinSpanTree);
}

static void PrimMinSpanTree_FailedMalloc()
{
    FailedMallocTest(PrimMinSpanTree);
}

static void PrimMinSpanTree_CalculatesMst()
{
    CalculatesMstTest(PrimMinSpanTree);
}

static void PrimMinSpanTree_ClearsInitalMstValues()
{
    ClearsInitialMstValuesTest(PrimMinSpanTree);
}

static void PrimMinSpanTree_ArithmeticOverflow()
{
    ArithmeticOverflowTest(PrimMinSpanTree);
}

static void MinSpanTree_Destroy_NullParameter() { MinSpanTree_Destroy(NULL); }

static void MinSpanTree_Edges_Destroy_NullParameter()
{
    MinSpanTree_Edges_Destroy(NULL);
}

int register_min_spanning_tree_tests()
{
    CU_TestInfo PrimTestsNaive[] = { CU_TEST_INFO(
                                         PrimMinSpanTreeNaive_NullParameter),
        CU_TEST_INFO(PrimMinSpanTreeNaive_ArithmeticOverflow),
        CU_TEST_INFO(PrimMinSpanTreeNaive_FailedMalloc),
        CU_TEST_INFO(PrimMinSpanTreeNaive_ClearsInitalMstValues),
        CU_TEST_INFO(PrimMinSpanTreeNaive_CalculatesMst), CU_TEST_INFO_NULL };

    CU_TestInfo PrimTests[] = { CU_TEST_INFO(PrimMinSpanTree_NullParameter),
        CU_TEST_INFO(PrimMinSpanTree_ArithmeticOverflow),
        CU_TEST_INFO(PrimMinSpanTree_FailedMalloc),
        CU_TEST_INFO(PrimMinSpanTree_ClearsInitalMstValues),
        CU_TEST_INFO(PrimMinSpanTree_CalculatesMst), CU_TEST_INFO_NULL };

    CU_TestInfo DestroyTests[]
        = { CU_TEST_INFO(MinSpanTree_Destroy_NullParameter),
              CU_TEST_INFO(MinSpanTree_Edges_Destroy_NullParameter),
              CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "PrimMinSpanTreeNaive",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = PrimTestsNaive },
        { .pName = "PrimMinSpanTree",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = PrimTests },
        { .pName = "MinSpanTree_Destroy",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = DestroyTests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}