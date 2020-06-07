#include "./random_contraction.h"

#include <stdlib.h>
#include <time.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#define CU_TEST_INFO(test_func) \
  { #test_func, test_func }

static int noop(void) {
  srand(time(0));
  return 0;
}

/*************************** RC_KargerMinCut **********************************/
static void RC_KargerMinCut_null() {
  Deprecated_Graph* result = RC_KargerMinCut(NULL);
  CU_ASSERT_PTR_NULL(result);
}

static void RC_KargerMinCut_small_graph() {
  Deprecated_Graph* graph = Deprecated_Graph_FromFile(
      "src/random_contraction/test_data/graph-4-2.txt");

  Deprecated_Graph* result = RC_KargerMinCut(graph);
  CU_ASSERT_EQUAL(2, result->m);

  Deprecated_Graph_Destroy(result);
  Deprecated_Graph_Destroy(graph);
}

static void RC_KargerMinCut_slightly_larger_graph() {
  Deprecated_Graph* graph = Deprecated_Graph_FromFile(
      "src/random_contraction/test_data/graph-8-2.txt");

  Deprecated_Graph* result = RC_KargerMinCut(graph);
  CU_ASSERT_EQUAL(2, result->m);

  Deprecated_Graph_Destroy(result);
  Deprecated_Graph_Destroy(graph);
}

static void RC_KargerMinCut_medium_graph() {
  Deprecated_Graph* graph = Deprecated_Graph_FromFile(
      "src/random_contraction/test_data/graph-25-6.txt");

  Deprecated_Graph* result = RC_KargerMinCut(graph);
  CU_ASSERT_EQUAL(6, result->m);

  Deprecated_Graph_Destroy(result);
  Deprecated_Graph_Destroy(graph);
}

/*************************** RC_KargerSteinMinCut *****************************/
static void RC_KargerSteinMinCut_null() {
  Deprecated_Graph* result = RC_KargerSteinMinCut(NULL);
  CU_ASSERT_PTR_NULL(result);
}

static void RC_KargerSteinMinCut_small_graph() {
  Deprecated_Graph* graph = Deprecated_Graph_FromFile(
      "src/random_contraction/test_data/graph-4-2.txt");

  Deprecated_Graph* result = RC_KargerSteinMinCut(graph);
  CU_ASSERT_EQUAL(2, result->m);

  Deprecated_Graph_Destroy(result);
  Deprecated_Graph_Destroy(graph);
}

static void RC_KargerSteinMinCut_medium_graph() {
  Deprecated_Graph* graph = Deprecated_Graph_FromFile(
      "src/random_contraction/test_data/graph-25-6.txt");

  Deprecated_Graph* result = RC_KargerSteinMinCut(graph);
  CU_ASSERT_EQUAL(6, result->m);

  Deprecated_Graph_Destroy(result);
  Deprecated_Graph_Destroy(graph);
}

static void RC_KargerSteinMinCut_large() {
  // Disabled b/c this tests takes a long time to run
  return;
  Deprecated_Graph* graph = Deprecated_Graph_FromFile(
      "src/random_contraction/test_data/graph-200-17.txt");

  Deprecated_Graph* result = RC_KargerSteinMinCut(graph);
  CU_ASSERT_EQUAL(17, result->m);

  Deprecated_Graph_Destroy(graph);
  Deprecated_Graph_Destroy(result);
}

int RegisterMinCutTests() {
  CU_TestInfo RC_KargerMinCut_tests[] = {
      CU_TEST_INFO(RC_KargerMinCut_null),
      CU_TEST_INFO(RC_KargerMinCut_small_graph),
      CU_TEST_INFO(RC_KargerMinCut_slightly_larger_graph),
      CU_TEST_INFO(RC_KargerMinCut_medium_graph), CU_TEST_INFO_NULL};

  CU_TestInfo RC_KargerSteinMinCut_tests[] = {
      CU_TEST_INFO(RC_KargerSteinMinCut_null),
      CU_TEST_INFO(RC_KargerSteinMinCut_small_graph),
      CU_TEST_INFO(RC_KargerSteinMinCut_medium_graph),
      CU_TEST_INFO(RC_KargerSteinMinCut_large), CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "RC_KargerMinCut suite",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = RC_KargerMinCut_tests},
                           {.pName = "RC_KargenSteinMinCut suite",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = RC_KargerSteinMinCut_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
