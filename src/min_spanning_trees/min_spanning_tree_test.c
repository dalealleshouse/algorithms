#include "./min_spanning_tree.h"

#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#include "../utils/malloc_test_wrapper.h"
#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

Graph* SimpleGraph() {
  return Graph_WeightedFromFile(5,
                                "src/min_spanning_trees/test_data/simple.txt");
}

Graph* ComplicatedGraph() {
  const static char* PATH = "src/min_spanning_trees/test_data/complicated.txt";
  return Graph_WeightedFromFileUndirected(PATH);
}

typedef Result (*min_span_tree)(const Graph* graph, MinSpanTree* mst);

// Common Test Functions
static void kNullParameterTest(min_span_tree minSpanTree) {
  Graph* input = SimpleGraph();

  // Both parameters NULL
  Result result = minSpanTree(NULL, NULL);
  CU_ASSERT_EQUAL(result, kNullParameter);

  // Result is null
  result = minSpanTree(input, NULL);
  CU_ASSERT_EQUAL(result, kNullParameter);

  // Input is NULL
  MinSpanTree mst;
  result = minSpanTree(NULL, &mst);
  CU_ASSERT_EQUAL(result, kNullParameter);

  Graph_Destroy(input, free);
}

static void FailedMallocTest(min_span_tree minSpanTree) {
  Graph* input = SimpleGraph();
  Result result;

  MinSpanTree mst;

  FAILED_MALLOC_TEST({ result = minSpanTree(input, &mst); });
  CU_ASSERT_EQUAL(result, kFailedMemoryAllocation);

  Graph_Destroy(input, free);
}

static void _validateSimpleGraphResults(const MinSpanTree* mst) {
  CU_ASSERT_PTR_NOT_NULL_FATAL(mst);
  CU_ASSERT_EQUAL(mst->cost, 7);
  CU_ASSERT_EQUAL(mst->edge_count, 3);
  Edge* edge = mst->edge_head;

  // first edge
  if (edge == NULL) {
    CU_FAIL("the first edge is NULL");
    return;
  }
  CU_ASSERT_EQUAL(edge->weight, 1);

  // second edge
  edge = edge->next;
  if (edge == NULL) {
    CU_FAIL("the second edge is NULL");
    return;
  }
  CU_ASSERT_EQUAL(edge->weight, 2);

  // third edge
  edge = edge->next;
  if (edge == NULL) {
    CU_FAIL("the third edge is NULL");
    return;
  }
  CU_ASSERT_EQUAL(edge->weight, 4);

  CU_ASSERT_PTR_NULL(edge->next);
}

static void CalculatesMstTest(min_span_tree minSpanTree) {
  MinSpanTree mst;
  Graph* input = SimpleGraph();

  Result result = minSpanTree(input, &mst);
  Graph_Destroy(input, free);

  CU_ASSERT_EQUAL(result, kSuccess);
  _validateSimpleGraphResults(&mst);

  MinSpanTree_Edges_Destroy(mst.edge_head);
}

static void ClearsInitialMstValuesTest(min_span_tree minSpanTree) {
  MinSpanTree mst;
  mst.cost = 5;
  Edge* edge = malloc(sizeof(Edge));
  edge->next = NULL;
  mst.edge_head = edge;

  Graph* input = SimpleGraph();

  Result result = minSpanTree(input, &mst);
  Graph_Destroy(input, free);
  free(edge);

  CU_ASSERT_EQUAL(result, kSuccess);
  _validateSimpleGraphResults(&mst);

  MinSpanTree_Edges_Destroy(mst.edge_head);
}

static void kArithmeticOverflowTest(min_span_tree minSpanTree) {
  Graph* input = SimpleGraph();

  for (size_t i = 1; i < input->n; i++) {
    Edge* e = input->V[i]->edges;
    while (e != NULL) {
      e->weight = INT_MAX;
      e = e->next;
    }
  }

  MinSpanTree mst = {
      .cost = 0, .edge_count = 0, .edge_head = NULL, .edge_tail = NULL};
  Result result = minSpanTree(input, &mst);
  CU_ASSERT_EQUAL(result, kArithmeticOverflow);

  Graph_Destroy(input, free);
  MinSpanTree_Edges_Destroy(mst.edge_head);
}

static void ComplicatedGraphTest(min_span_tree minSpanTree) {
  Graph* graph = ComplicatedGraph();

  MinSpanTree mst;
  Result result = minSpanTree(graph, &mst);
  CU_ASSERT_EQUAL(result, kSuccess);
  CU_ASSERT_EQUAL(mst.cost, -3612829);
  CU_ASSERT_EQUAL(mst.edge_count, 499);

  Graph_Destroy(graph, free);
  MinSpanTree_Edges_Destroy(mst.edge_head);
}

// PrimMinSpanTreeNaive

static void PrimMinSpanTreeNaive_kNullParameter() {
  kNullParameterTest(PrimMinSpanTreeNaive);
}

static void PrimMinSpanTreeNaive_FailedMalloc() {
  FailedMallocTest(PrimMinSpanTreeNaive);
}

static void PrimMinSpanTreeNaive_CalculatesMst() {
  CalculatesMstTest(PrimMinSpanTreeNaive);
}

static void PrimMinSpanTreeNaive_ClearsInitalMstValues() {
  ClearsInitialMstValuesTest(PrimMinSpanTreeNaive);
}

static void PrimMinSpanTreeNaive_kArithmeticOverflow() {
  kArithmeticOverflowTest(PrimMinSpanTreeNaive);
}

static void PrimMinSpanTreeNaive_ComplicatedGraph() {
  ComplicatedGraphTest(PrimMinSpanTreeNaive);
}

// PrimMinSpanTree
static void PrimMinSpanTree_kNullParameter() {
  kNullParameterTest(PrimMinSpanTree);
}

static void PrimMinSpanTree_FailedMalloc() {
  FailedMallocTest(PrimMinSpanTree);
}

static void PrimMinSpanTree_CalculatesMst() {
  CalculatesMstTest(PrimMinSpanTree);
}

static void PrimMinSpanTree_ClearsInitalMstValues() {
  ClearsInitialMstValuesTest(PrimMinSpanTree);
}

static void PrimMinSpanTree_kArithmeticOverflow() {
  kArithmeticOverflowTest(PrimMinSpanTree);
}

static void PrimMinSpanTree_ComplicatedGraph() {
  ComplicatedGraphTest(PrimMinSpanTree);
}

// KruskalMinSpanTree
static void KruskalMinSpanTree_kNullParameter() {
  kNullParameterTest(KruskalMinSpanTree);
}

static void KruskalMinSpanTree_FailedMalloc() {
  FailedMallocTest(KruskalMinSpanTree);
}

static void KruskalMinSpanTree_CalculatesMst() {
  CalculatesMstTest(KruskalMinSpanTree);
}

static void KruskalMinSpanTree_ClearsInitalMstValues() {
  ClearsInitialMstValuesTest(KruskalMinSpanTree);
}

static void KruskalMinSpanTree_kArithmeticOverflow() {
  kArithmeticOverflowTest(KruskalMinSpanTree);
}

static void KruskalMinSpanTree_ComplicatedGraph() {
  ComplicatedGraphTest(KruskalMinSpanTree);
}

static void MinSpanTree_Destroy_kNullParameter() { MinSpanTree_Destroy(NULL); }

static void MinSpanTree_Edges_Destroy_kNullParameter() {
  MinSpanTree_Edges_Destroy(NULL);
}

int RegisterMinSpanningTreeTests() {
  CU_TestInfo PrimTestsNaive[] = {
      CU_TEST_INFO(PrimMinSpanTreeNaive_kNullParameter),
      CU_TEST_INFO(PrimMinSpanTreeNaive_kArithmeticOverflow),
      CU_TEST_INFO(PrimMinSpanTreeNaive_FailedMalloc),
      CU_TEST_INFO(PrimMinSpanTreeNaive_ClearsInitalMstValues),
      CU_TEST_INFO(PrimMinSpanTreeNaive_CalculatesMst),
      CU_TEST_INFO(PrimMinSpanTreeNaive_ComplicatedGraph),
      CU_TEST_INFO_NULL};

  CU_TestInfo PrimTests[] = {
      CU_TEST_INFO(PrimMinSpanTree_kNullParameter),
      CU_TEST_INFO(PrimMinSpanTree_kArithmeticOverflow),
      CU_TEST_INFO(PrimMinSpanTree_FailedMalloc),
      CU_TEST_INFO(PrimMinSpanTree_ClearsInitalMstValues),
      CU_TEST_INFO(PrimMinSpanTree_CalculatesMst),
      CU_TEST_INFO(PrimMinSpanTree_ComplicatedGraph),
      CU_TEST_INFO_NULL};

  CU_TestInfo KruskalTests[] = {
      CU_TEST_INFO(KruskalMinSpanTree_kNullParameter),
      CU_TEST_INFO(KruskalMinSpanTree_kArithmeticOverflow),
      CU_TEST_INFO(KruskalMinSpanTree_FailedMalloc),
      CU_TEST_INFO(KruskalMinSpanTree_ClearsInitalMstValues),
      CU_TEST_INFO(KruskalMinSpanTree_CalculatesMst),
      CU_TEST_INFO(KruskalMinSpanTree_ComplicatedGraph),
      CU_TEST_INFO_NULL};

  CU_TestInfo DestroyTests[] = {
      CU_TEST_INFO(MinSpanTree_Destroy_kNullParameter),
      CU_TEST_INFO(MinSpanTree_Edges_Destroy_kNullParameter),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "PrimMinSpanTreeNaive",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = PrimTestsNaive},
                           {.pName = "PrimMinSpanTree",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = PrimTests},
                           {.pName = "KruskalMinSpanTree",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = KruskalTests},
                           {.pName = "MinSpanTree_Destroy",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = DestroyTests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
