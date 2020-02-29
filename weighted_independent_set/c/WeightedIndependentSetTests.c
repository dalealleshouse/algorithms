#include <limits.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "WeightedIndependentSet.h"
#include "include/TestHelpers.h"

typedef Result (*wis)(PathGraph* graph, WeightedIndependentSet** conclusion);

static void _containsId(WeightedVertex* vertices[], const size_t n,
                        const unsigned int id) {
  for (size_t i = 0; i < n; i++) {
    if (id == vertices[i]->id) return;
  }

  CU_FAIL("ID not found");
}

static void _test_NullParameter(wis func) {
  Result result = func(NULL, NULL);
  CU_ASSERT_EQUAL(result, NullParameter);
}

static void _test_EmptySet(wis func) {
  WeightedIndependentSet* wis = NULL;
  PathGraph graph;
  graph.n = 0;
  graph.vertices = NULL;

  Result result = func(&graph, &wis);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_PTR_EQUAL(wis, &EMPTY_SET);
}

static void _test_SingleVertex(wis func) {
  unsigned int ids[] = {1};
  int weights[] = {100};
  PathGraph* graph;
  PathGraph_Create(ids, weights, 1, &graph);
  WeightedIndependentSet* wis = NULL;

  Result result = func(graph, &wis);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_EQUAL(1, wis->n);
  CU_ASSERT_EQUAL(100, wis->weight);
  CU_ASSERT_EQUAL(1, wis->vertices[0]->id);
  CU_ASSERT_EQUAL(100, wis->vertices[0]->weight);

  PathGraph_Destroy(graph);
  WeightedIndependentSet_Destroy(wis);
}

static void _test_TwoVertices(wis func) {
  unsigned int ids[] = {1, 2};
  int weights[] = {100, 101};
  PathGraph* graph;
  PathGraph_Create(ids, weights, 2, &graph);
  WeightedIndependentSet* wis = NULL;

  Result result = func(graph, &wis);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_EQUAL(1, wis->n);
  CU_ASSERT_EQUAL(101, wis->weight);
  CU_ASSERT_EQUAL(2, wis->vertices[0]->id);
  CU_ASSERT_EQUAL(101, wis->vertices[0]->weight);

  WeightedIndependentSet_Destroy(wis);
  PathGraph_Destroy(graph);
}

static void _test_FourVertices(wis func) {
  unsigned int ids[] = {0, 1, 2, 3};
  int weights[] = {1, 4, 5, 4};
  PathGraph* graph;
  PathGraph_Create(ids, weights, 4, &graph);
  WeightedIndependentSet* wis = NULL;

  Result result = func(graph, &wis);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_EQUAL(2, wis->n);
  CU_ASSERT_EQUAL(8, wis->weight);
  _containsId(wis->vertices, wis->n, 1);
  _containsId(wis->vertices, wis->n, 3);

  WeightedIndependentSet_Destroy(wis);
  PathGraph_Destroy(graph);
}

static void _test_SixVertices(wis func) {
  unsigned int ids[] = {0, 1, 2, 3, 4, 5};
  int weights[] = {3, 2, 1, 6, 4, 5};
  PathGraph* graph;
  PathGraph_Create(ids, weights, 6, &graph);
  WeightedIndependentSet* wis = NULL;

  Result result = func(graph, &wis);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_EQUAL(3, wis->n);
  CU_ASSERT_EQUAL(14, wis->weight);
  _containsId(wis->vertices, wis->n, 0);
  _containsId(wis->vertices, wis->n, 3);
  _containsId(wis->vertices, wis->n, 5);

  WeightedIndependentSet_Destroy(wis);
  PathGraph_Destroy(graph);
}

static void _test_ArimiticOverflow(wis func) {
  unsigned int ids[] = {0, 1, 2, 3};
  int weights[] = {1, UINT_MAX, 5, 4};
  PathGraph* graph;
  PathGraph_Create(ids, weights, 4, &graph);
  WeightedIndependentSet* wis = NULL;

  Result result = func(graph, &wis);
  CU_ASSERT_EQUAL(result, ArithmeticOverflow);

  WeightedIndependentSet_Destroy(wis);
  PathGraph_Destroy(graph);
}

static void WeightedIndependentSet_Recursive_NullParameter() {
  _test_NullParameter(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_EmptySet() {
  _test_EmptySet(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_SingleVertex() {
  _test_SingleVertex(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_TwoVertices() {
  _test_TwoVertices(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_FourVertices() {
  _test_FourVertices(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_SixVertices() {
  _test_SixVertices(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_ArimiticOverflow() {
  _test_ArimiticOverflow(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Dynamic_NullParameter() {
  _test_NullParameter(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_EmptySet() {
  _test_EmptySet(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_SingleVertex() {
  _test_SingleVertex(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_TwoVertices() {
  _test_TwoVertices(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_FourVertices() {
  _test_FourVertices(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_SixVertices() {
  _test_SixVertices(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_ArimiticOverflow() {
  _test_ArimiticOverflow(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_NullParameter() {
  _test_NullParameter(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_EmptySet() {
  _test_EmptySet(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_SingleVertex() {
  _test_SingleVertex(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_TwoVertices() {
  _test_TwoVertices(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_FourVertices() {
  _test_FourVertices(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_SixVertices() {
  _test_SixVertices(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_ArimiticOverflow() {
  _test_ArimiticOverflow(WeightedIndependentSet_Dynamic_Reconstruction);
}

int registerWeightedIndependentSetTests() {
  CU_TestInfo WeightedIndependentSet_Recursive_Tests[] = {
      CU_TEST_INFO(WeightedIndependentSet_Recursive_NullParameter),
      CU_TEST_INFO(WeightedIndependentSet_Recursive_EmptySet),
      CU_TEST_INFO(WeightedIndependentSet_Recursive_SingleVertex),
      CU_TEST_INFO(WeightedIndependentSet_Recursive_TwoVertices),
      CU_TEST_INFO(WeightedIndependentSet_Recursive_FourVertices),
      CU_TEST_INFO(WeightedIndependentSet_Recursive_SixVertices),
      CU_TEST_INFO(WeightedIndependentSet_Recursive_ArimiticOverflow),
      CU_TEST_INFO_NULL};

  CU_TestInfo WeightedIndependentSet_Dynamic_Tests[] = {
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_NullParameter),
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_EmptySet),
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_SingleVertex),
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_TwoVertices),
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_FourVertices),
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_SixVertices),
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_ArimiticOverflow),
      CU_TEST_INFO_NULL};

  CU_TestInfo WeightedIndependentSet_Dynamic_Reconstruction_Tests[] = {
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_Reconstruction_NullParameter),
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_Reconstruction_EmptySet),
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_Reconstruction_SingleVertex),
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_Reconstruction_TwoVertices),
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_Reconstruction_FourVertices),
      CU_TEST_INFO(WeightedIndependentSet_Dynamic_Reconstruction_SixVertices),
      CU_TEST_INFO(
          WeightedIndependentSet_Dynamic_Reconstruction_ArimiticOverflow),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {
      {.pName = "Weighted Independent Set Recursive",
       .pInitFunc = noop,
       .pCleanupFunc = noop,
       .pTests = WeightedIndependentSet_Recursive_Tests},
      {.pName = "Weighted Independent Set Dynamic",
       .pInitFunc = noop,
       .pCleanupFunc = noop,
       .pTests = WeightedIndependentSet_Dynamic_Tests},
      {.pName = "Weighted Independent Set Dynamic Reconstruction",
       .pInitFunc = noop,
       .pCleanupFunc = noop,
       .pTests = WeightedIndependentSet_Dynamic_Reconstruction_Tests},
      CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
