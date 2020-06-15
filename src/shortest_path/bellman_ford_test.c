#include "./bellman_ford.h"

#include <limits.h>
#include <stdlib.h>

#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

const static size_t small_n = 6;
const static char* small_path =
    "src/shortest_path/test_data/small_negative_weight.txt";

const static size_t neg_n = 6;
const static char* neg_path =
    "src/shortest_path/test_data/small_negative_cycle.txt";

static int _value(Vertex* v) {
  if (v == NULL || v->data == NULL) return -1;

  return ((BFData*)v->data)->distance;
}

static void _testPath(const size_t n, const vertex_id expected[n],
                      const Path* path) {
  size_t tracker = n;

  while (tracker > 0) {
    CU_ASSERT_EQUAL(path->n, tracker);
    CU_ASSERT_EQUAL(path->vertex, expected[n - tracker]);

    path = path->next;
    tracker--;
  }
}

static void _testPathFromFile(const char* path, const size_t n,
                              const vertex_id expected[n]) {
  Graph* sut = Graph_WeightedFromFileDirected(path);
  GraphResult result = BellmanFordShortestPath(sut, 4);
  CU_ASSERT_EQUAL(result, Graph_kSuccess);

  Path* solution = NULL;
  result = BellmandFordTracePath(sut, expected[n - 1], &solution);
  CU_ASSERT_EQUAL(result, Graph_kSuccess);

  _testPath(n, expected, solution);

  Path_Destroy(solution);
  Graph_Destroy(sut, free);
}

static void BellmanFordShortestPath_NullParamter() {
  GraphResult result = BellmanFordShortestPath(NULL, 0);
  CU_ASSERT_EQUAL(result, Graph_kNullParameter);
}

static void BellmanFordShortestPath_InvalidStartVertex() {
  Graph* sut = Graph_WeightedFromFile(small_n, small_path);

  GraphResult result = BellmanFordShortestPath(sut, small_n);
  CU_ASSERT_EQUAL(result, Graph_InvalidVertexId);

  Graph_Destroy(sut, NULL);
}

static void BellmanFordShortestPath_HappyPath() {
  Graph* sut = Graph_WeightedFromFile(small_n, small_path);

  GraphResult result = BellmanFordShortestPath(sut, 1);
  CU_ASSERT_EQUAL(result, Graph_kSuccess);

  CU_ASSERT_EQUAL(UNINITIALIZED, _value(sut->V[0]));
  CU_ASSERT_EQUAL(0, _value(sut->V[1]));
  CU_ASSERT_EQUAL(1, _value(sut->V[2]));
  CU_ASSERT_EQUAL(5, _value(sut->V[3]));
  CU_ASSERT_EQUAL(4, _value(sut->V[4]));
  CU_ASSERT_EQUAL(2, _value(sut->V[5]));

  Graph_Destroy(sut, free);
}

static void BellmanFordShortestPath_NegativeCycle() {
  Graph* sut = Graph_WeightedFromFile(neg_n, neg_path);

  GraphResult result = BellmanFordShortestPath(sut, 1);
  CU_ASSERT_EQUAL(result, Graph_NegativeCycle);

  Graph_Destroy(sut, NULL);
}

static void BellmanFordShortestPath_kArithmeticOverflow() {
  Graph* sut = Graph_WeightedFromFile(small_n, small_path);

  sut->V[3]->in_edges->weight = INT_MAX;

  GraphResult result = BellmanFordShortestPath(sut, 1);
  CU_ASSERT_EQUAL(result, Graph_kArithmeticOverflow);

  Graph_Destroy(sut, free);
}

static void BellmandFordTracePath_kNullParameter() {
  GraphResult result = BellmandFordTracePath(NULL, 1, NULL);
  CU_ASSERT_EQUAL(result, Graph_kNullParameter);
}

static void BellmandFordTracePath_InvalidVertex() {
  const char* path = "src/shortest_path/test_data/16_16.txt";
  Graph* sut = Graph_WeightedFromFileDirected(path);
  Path* solution = NULL;
  GraphResult result = BellmandFordTracePath(sut, 32, &solution);
  CU_ASSERT_EQUAL(result, Graph_VertexIdExceedsMaxSize);

  Graph_Destroy(sut, free);
}

static void BellmandFordTracePath_NullData() {
  const char* path = "src/shortest_path/test_data/16_16.txt";
  Graph* sut = Graph_WeightedFromFileDirected(path);

  Path* solution = NULL;
  GraphResult result = BellmandFordTracePath(sut, 5, &solution);

  CU_ASSERT_EQUAL(result, Graph_kSuccess);
  CU_ASSERT_EQUAL(solution->vertex, 5);
  CU_ASSERT_PTR_NULL(solution->next);

  Path_Destroy(solution);
  Graph_Destroy(sut, free);
}

static void BellmandFordTracePath_FromFiles() {
  vertex_id expected[] = {4, 5, 9, 11, 16};
  _testPathFromFile("src/shortest_path/test_data/16_16.txt", 5, expected);

  vertex_id expected2[] = {4, 7, 13, 17, 23, 28, 30};
  _testPathFromFile("src/shortest_path/test_data/18_32.txt", 7, expected2);

  vertex_id expected3[] = {4,   15,  34,  47,  53,  68,  70,  73,  87,
                           103, 124, 155, 172, 188, 225, 227, 236, 252};
  _testPathFromFile("src/shortest_path/test_data/30_256.txt", 18, expected3);
}

int RegisterBellmanFordTests() {
  CU_TestInfo shortest_path[] = {
      CU_TEST_INFO(BellmanFordShortestPath_NullParamter),
      CU_TEST_INFO(BellmanFordShortestPath_InvalidStartVertex),
      CU_TEST_INFO(BellmanFordShortestPath_HappyPath),
      CU_TEST_INFO(BellmanFordShortestPath_NegativeCycle),
      CU_TEST_INFO(BellmanFordShortestPath_kArithmeticOverflow),
      CU_TEST_INFO_NULL};

  CU_TestInfo trace_path[] = {
      CU_TEST_INFO(BellmandFordTracePath_kNullParameter),
      CU_TEST_INFO(BellmandFordTracePath_InvalidVertex),
      CU_TEST_INFO(BellmandFordTracePath_NullData),
      CU_TEST_INFO(BellmandFordTracePath_FromFiles), CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Bellman Ford",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = shortest_path},
                           {.pName = "Bellman Ford Trace Path",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = trace_path},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
