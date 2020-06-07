#include "./graph.h"

#include <stdlib.h>

#include "../utils/malloc_test_wrapper.h"
#include "./error_reporter.h"
#include "./test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

static Graph* _graph;

/*************************** Graph_Create *************************************/
static void Graph_Create_malloc_tester(void) {
  Graph* graph = Graph_Create(1);
  CU_ASSERT_PTR_NULL(graph);
  CU_ASSERT_EQUAL(Graph_FailedMemoryAllocation, ErrorReporter_LastErrorCode());
}

static void Graph_Create_failed_malloc() {
  FAILED_MALLOC_TEST({ Graph_Create_malloc_tester(); });
}

static void Graph_Create_initalizes_values() {
  size_t n = 10;
  Graph* graph = Graph_Create(n);

  if (graph == NULL) {
    CU_FAIL("graph is NULL");
    return;
  }

  CU_ASSERT_EQUAL(0, graph->m);
  CU_ASSERT_EQUAL(n, graph->n);

  if (graph->V == NULL) {
    CU_FAIL("graph->V is NULL");
    return;
  }

  for (size_t i = 0; i < n; i++) CU_ASSERT_PTR_NOT_NULL(graph->V[i]);

  Graph_Destroy(graph, NULL);
}

/*************************** Graph_AddEdge ************************************/
static void Graph_AddEdge_malloc_tester(void) {
  GraphResult result = Graph_AddEdge(_graph, 1, 2);
  CU_ASSERT_EQUAL(Graph_FailedMemoryAllocation, result);
}

static void Graph_AddEdge_failed_malloc() {
  _graph = Graph_Create(10);
  FAILED_MALLOC_TEST({ Graph_AddEdge_malloc_tester(); });
  Graph_Destroy(_graph, NULL);
}

static void Graph_AddEdge_null_parameter() {
  GraphResult result = Graph_AddEdge(NULL, 1, 2);
  CU_ASSERT_EQUAL(Graph_NullParameter, result);
}

static void Graph_AddEdge_invalid_head() {
  Graph* graph = Graph_Create(10);

  GraphResult result = Graph_AddEdge(graph, 10, 1);
  CU_ASSERT_EQUAL(Graph_VertexIdExceedsMaxSize, result);

  Graph_Destroy(graph, NULL);
}

static void Graph_AddEdge_invalid_tail() {
  Graph* graph = Graph_Create(10);

  GraphResult result = Graph_AddEdge(graph, 1, 10);
  CU_ASSERT_EQUAL(Graph_VertexIdExceedsMaxSize, result);

  Graph_Destroy(graph, NULL);
}

static void Graph_AddEdge_intializes_values() {
  Graph* graph = Graph_Create(10);

  GraphResult result = Graph_AddEdge(graph, 1, 2);

  CU_ASSERT_EQUAL(Graph_Success, result);
  if (graph->V[2]->edges == NULL) {
    CU_FAIL("graph->V[2]->edges is NULL");
    return;
  }

  CU_ASSERT_EQUAL(1, graph->V[2]->edges->head);
  CU_ASSERT_EQUAL(1, graph->m);

  // vertex degree
  CU_ASSERT_EQUAL(0, graph->V[1]->out_degree);
  CU_ASSERT_EQUAL(1, graph->V[1]->in_degree);

  CU_ASSERT_EQUAL(1, graph->V[2]->out_degree);
  CU_ASSERT_EQUAL(0, graph->V[2]->in_degree);

  Graph_Destroy(graph, NULL);
}

static void Graph_AddEdge_sets_incoming() {
  Graph* graph = Graph_Create(10);

  GraphResult result = Graph_AddEdge(graph, 1, 2);

  CU_ASSERT_EQUAL(Graph_Success, result);
  if (graph->V[1]->in_edges == NULL) {
    CU_FAIL("graph->V[1]->in_edges is NULL");
    return;
  }

  CU_ASSERT_EQUAL(2, graph->V[1]->in_edges->tail);

  Graph_Destroy(graph, NULL);
}

static void Graph_AddEdge_sets_weight_to_1() {
  Graph* graph = Graph_Create(10);

  GraphResult result = Graph_AddEdge(graph, 1, 2);

  CU_ASSERT_EQUAL(Graph_Success, result);
  if (graph->V[1]->in_edges == NULL) {
    CU_FAIL("graph->V[1]->in_edges is NULL");
    return;
  }

  CU_ASSERT_PTR_NULL(graph->V[1]->edges);

  if (graph->V[2]->edges == NULL) {
    CU_FAIL("graph->V[2]->edges is NULL");
    return;
  }
  CU_ASSERT_EQUAL(1, graph->V[1]->in_edges->weight);
  CU_ASSERT_EQUAL(1, graph->V[2]->edges->weight);

  Graph_Destroy(graph, NULL);
}

static void Graph_AddEdge_two_edges() {
  Graph* graph = Graph_Create(10);

  GraphResult result = Graph_AddEdge(graph, 1, 2);
  CU_ASSERT_EQUAL(Graph_Success, result);

  result = Graph_AddEdge(graph, 1, 3);
  CU_ASSERT_EQUAL(Graph_Success, result);

  CU_ASSERT_EQUAL(graph->V[1]->in_degree, 2);

  // edge 1
  CU_ASSERT_EQUAL(graph->V[1]->in_edges->next->weight, 1);
  CU_ASSERT_EQUAL(graph->V[1]->in_edges->next->head, 1);
  CU_ASSERT_EQUAL(graph->V[1]->in_edges->next->tail, 2);

  // edge 2
  CU_ASSERT_EQUAL(graph->V[1]->in_edges->weight, 1);
  CU_ASSERT_EQUAL(graph->V[1]->in_edges->head, 1);
  CU_ASSERT_EQUAL(graph->V[1]->in_edges->tail, 3);

  Graph_Destroy(graph, NULL);
}

/*************************** Graph_AddWeightedEdge ****************************/
// Because edge just calls weighted edge, we can assume all the standard tests
// work e.g. failed malloc, invalid head, etc...
static void Graph_AddWeightedEdge_sets_weight() {
  const int expected = 5;
  Graph* graph = Graph_Create(10);

  GraphResult result = Graph_AddWeightedEdge(graph, 1, 2, expected);

  CU_ASSERT_EQUAL(Graph_Success, result);
  CU_ASSERT_EQUAL(expected, graph->V[1]->in_edges->weight);
  CU_ASSERT_EQUAL(expected, graph->V[2]->edges->weight);

  Graph_Destroy(graph, NULL);
}

/*************************** Graph_FromFile ***********************************/
static void Graph_FromFile_null_parameter() {
  Graph* graph = Graph_FromFile(10, NULL);
  CU_ASSERT_EQUAL(Graph_NullParameter, ErrorReporter_LastErrorCode());
  CU_ASSERT_PTR_NULL(graph);
}

static void Graph_FromFile_malloc_tester(void) {
  Graph* graph = Graph_FromFile(6, "src/utils/test_data/graph-4-2.txt");
  CU_ASSERT_EQUAL(Graph_FailedMemoryAllocation, ErrorReporter_LastErrorCode());
  CU_ASSERT_PTR_NULL(graph);
}

static void Graph_FromFile_failed_malloc() {
  FAILED_MALLOC_TEST({ Graph_FromFile_malloc_tester(); });
}

static void Graph_FromFile_invalid_path() {
  Graph* graph = Graph_FromFile(10, "bad_path.txt");
  CU_ASSERT_EQUAL(Graph_InvalidFilePath, ErrorReporter_LastErrorCode());
  CU_ASSERT_PTR_NULL(graph);
}

static void Graph_FromFile_insufficent_size() {
  Graph* graph = Graph_FromFile(5, "src/utils/test_data/graph-4-2.txt");
  CU_ASSERT_PTR_NULL(graph);
  CU_ASSERT_EQUAL(Graph_VertexIdExceedsMaxSize, ErrorReporter_LastErrorCode());

  Graph_Destroy(graph, NULL);
}

static void Graph_FromFile_bad_data() {
  Graph* graph = Graph_FromFile(3, "src/utils/test_data/bad_graph.txt");
  CU_ASSERT_PTR_NOT_NULL(graph);
  Graph_Destroy(graph, NULL);
}

static void AllEdgeWeightsAreOne(Edge* e) {
  while (e != NULL) {
    CU_ASSERT_EQUAL(1, e->weight);
    e = e->next;
  }
}

static void Graph_FromFile_standard() {
  const int n = 6;
  Graph* graph = Graph_FromFile(n, "src/utils/test_data/graph-4-2.txt");

  if (graph == NULL) {
    CU_FAIL("graph is NULL");
    return;
  }
  CU_ASSERT_EQUAL(6, graph->n);
  CU_ASSERT_EQUAL(10, graph->m);

  for (size_t i = 0; i < n; i++) {
    AllEdgeWeightsAreOne(graph->V[i]->edges);
    AllEdgeWeightsAreOne(graph->V[i]->in_edges);
  }

  Graph_Destroy(graph, NULL);
}

/*************************** Graph_WeightedFromFile ***************************/
static void EdgeIsValid(Edge* e, vertex_id head, vertex_id tail, int weight) {
  /* CU_ASSERT_PTR_NOT_NULL_FATAL(e); */
  CU_ASSERT_EQUAL(head, e->head);
  CU_ASSERT_EQUAL(tail, e->tail);
  CU_ASSERT_EQUAL(weight, e->weight);
}

static void Graph_WeightedFromFile_standard() {
  const int n = 5;
  const int m = 5;
  Graph* graph = Graph_WeightedFromFile(n, "src/utils/test_data/weighted.txt");

  if (graph == NULL) {
    CU_FAIL("graph is NULL");
    return;
  }

  CU_ASSERT_EQUAL(n, graph->n);
  CU_ASSERT_EQUAL(m, graph->m);

  EdgeIsValid(graph->V[1]->edges, 4, 1, 4);
  EdgeIsValid(graph->V[1]->edges->next, 2, 1, 1);
  EdgeIsValid(graph->V[2]->edges, 3, 2, 6);
  EdgeIsValid(graph->V[2]->edges->next, 4, 2, 2);
  CU_ASSERT_PTR_NULL(graph->V[3]->edges);
  EdgeIsValid(graph->V[4]->edges, 3, 4, 3);

  CU_ASSERT_PTR_NULL(graph->V[1]->in_edges);
  EdgeIsValid(graph->V[2]->in_edges, 2, 1, 1);
  EdgeIsValid(graph->V[3]->in_edges, 3, 4, 3);
  EdgeIsValid(graph->V[3]->in_edges->next, 3, 2, 6);
  EdgeIsValid(graph->V[4]->in_edges, 4, 2, 2);
  EdgeIsValid(graph->V[4]->in_edges->next, 4, 1, 4);

  Graph_Destroy(graph, NULL);
}

static void Graph_WeightedFromFile_big() {
  const int n = 201;
  Graph* sut =
      Graph_WeightedFromFile(n, "src/utils/test_data/big_weighted.txt");

  CU_ASSERT_EQUAL(n, sut->n);
  CU_ASSERT_EQUAL(3934, sut->m);

  Graph_Destroy(sut, NULL);
}

/*************************** Graph_Destroy ************************************/
static void Graph_Destroy_complex_graph() {
  const size_t n = 10;
  Graph* graph = Graph_Create(n);
  for (size_t i = 0; i < n; i++) {
    TestThingy* thing = malloc(sizeof(TestThingy));
    graph->V[i]->data = thing;
  }

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      if (j == i) continue;

      GraphResult result = Graph_AddEdge(graph, i, j);
      CU_ASSERT_EQUAL(Graph_Success, result);
    }
  }

  CU_ASSERT_EQUAL(n, graph->n);
  CU_ASSERT_EQUAL(90, graph->m);

  Graph_Destroy(graph, free);
}

int RegisterGraphTests() {
  CU_TestInfo Queue_tests[] = {CU_TEST_INFO(Graph_Create_failed_malloc),
                               CU_TEST_INFO(Graph_Create_initalizes_values),
                               CU_TEST_INFO(Graph_AddEdge_failed_malloc),
                               CU_TEST_INFO(Graph_AddEdge_null_parameter),
                               CU_TEST_INFO(Graph_AddEdge_invalid_head),
                               CU_TEST_INFO(Graph_AddEdge_invalid_tail),
                               CU_TEST_INFO(Graph_AddEdge_intializes_values),
                               CU_TEST_INFO(Graph_AddEdge_sets_incoming),
                               CU_TEST_INFO(Graph_AddEdge_sets_weight_to_1),
                               CU_TEST_INFO(Graph_AddEdge_two_edges),
                               CU_TEST_INFO(Graph_AddWeightedEdge_sets_weight),
                               CU_TEST_INFO(Graph_FromFile_failed_malloc),
                               CU_TEST_INFO(Graph_FromFile_null_parameter),
                               CU_TEST_INFO(Graph_FromFile_invalid_path),
                               CU_TEST_INFO(Graph_FromFile_insufficent_size),
                               CU_TEST_INFO(Graph_FromFile_bad_data),
                               CU_TEST_INFO(Graph_FromFile_standard),
                               CU_TEST_INFO(Graph_WeightedFromFile_standard),
                               CU_TEST_INFO(Graph_WeightedFromFile_big),
                               CU_TEST_INFO(Graph_Destroy_complex_graph),
                               CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Graph",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Queue_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
