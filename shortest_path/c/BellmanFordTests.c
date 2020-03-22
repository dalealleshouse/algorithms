#include <limits.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/GraphSearch.h"
#include "include/TestHelpers.h"

#include "BellmanFord.h"

const static size_t small_n = 6;
const static char* small_path = "src/graphs/small_negative_weight.txt";

const static size_t neg_n = 6;
const static char* neg_path = "src/graphs/small_negative_cycle.txt";

static int _value(Vertex* v) {
  if (v == NULL || v->data == NULL) return -1;

  return ((Distance*)v->data)->distance;
}

static void BellmanFordShortestPath_NullParamter() {
  GraphResult result = BellmanFordShortestPath(NULL, 0);
  CU_ASSERT_EQUAL(result, Graph_NullParameter);
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
  CU_ASSERT_EQUAL(result, Graph_Success);

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

static void BellmanFordShortestPath_ArithmeticOverflow() {
  Graph* sut = Graph_WeightedFromFile(small_n, small_path);

  sut->V[3]->in_edges->weight = INT_MAX;

  GraphResult result = BellmanFordShortestPath(sut, 1);
  CU_ASSERT_EQUAL(result, Graph_ArithmeticOverflow);

  Graph_Destroy(sut, NULL);
}

int register_bf_tests() {
  CU_TestInfo tests[] = {
      CU_TEST_INFO(BellmanFordShortestPath_NullParamter),
      CU_TEST_INFO(BellmanFordShortestPath_InvalidStartVertex),
      CU_TEST_INFO(BellmanFordShortestPath_HappyPath),
      CU_TEST_INFO(BellmanFordShortestPath_NegativeCycle),
      CU_TEST_INFO(BellmanFordShortestPath_ArithmeticOverflow),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Belmman Ford",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
