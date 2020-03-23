#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "KruskalCluster.h"
#include "include/Graph.h"
#include "include/ResultCode.h"
#include "include/TestHelpers.h"

static Graph* SmallGraph() {
  return Graph_WeightedFromFile(5, "src/data/simple.txt");
}

static Graph* MediumCluster() {
  const static char* PATH = "src/data/cluster_med.txt";
  return Graph_WeightedFromFileUndirected(PATH);
}

static Graph* BigCluster() {
  const static char* PATH = "src/data/cluster_20_128.txt";
  return Graph_WeightedFromFileUndirected(PATH);
}

static Graph* GiantCluster() {
  const static char* PATH = "src/data/clustering1.txt";
  return Graph_WeightedFromFileUndirected(PATH);
}

// Common Test Functions
static void KruskalCluster_NullParameterTest() {
  Result result = KruskalCluster(NULL, 1, NULL);
  CU_ASSERT_EQUAL(result, NullParameter);
}

static void KruskalCluster_CalcsMaxSpacingForMedCluster() {
  Graph* graph = MediumCluster();

  int max_space;
  Result result = KruskalCluster(graph, 4, &max_space);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_EQUAL(max_space, 90);

  Graph_Destroy(graph, NULL);
}

static void KruskalCluster_CalcsMaxSpacingForBigCluster() {
  Graph* graph = BigCluster();

  int max_space;
  Result result = KruskalCluster(graph, 4, &max_space);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_EQUAL(max_space, 578);

  Graph_Destroy(graph, NULL);
}

static void KruskalCluster_CalcsMaxSpacingForGiantCluster() {
  Graph* graph = GiantCluster();

  int max_space;
  Result result = KruskalCluster(graph, 4, &max_space);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_EQUAL(max_space, 106);

  Graph_Destroy(graph, NULL);
}

static void KruskalCluster_FailedMallocTest() {
  Graph* input = SmallGraph();
  Result result;
  int max_space;

  FAILED_MALLOC_TEST({ result = KruskalCluster(input, 2, &max_space); });
  CU_ASSERT_EQUAL(result, FailedMemoryAllocation);

  Graph_Destroy(input, free);
}

static void KruskalCluster_InvalidInput() {
  Graph* input = SmallGraph();
  int max_space;

  Result result = KruskalCluster(input, 1, &max_space);
  CU_ASSERT_EQUAL(result, ArgumentOutOfRange);

  Graph_Destroy(input, free);
}

int register_kruskal_cluster_tests() {
  CU_TestInfo KruskalClusterTests[] = {
      CU_TEST_INFO(KruskalCluster_NullParameterTest),
      CU_TEST_INFO(KruskalCluster_CalcsMaxSpacingForMedCluster),
      CU_TEST_INFO(KruskalCluster_CalcsMaxSpacingForBigCluster),
      CU_TEST_INFO(KruskalCluster_CalcsMaxSpacingForGiantCluster),
      CU_TEST_INFO(KruskalCluster_FailedMallocTest),
      CU_TEST_INFO(KruskalCluster_InvalidInput),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "KruskalCluster",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = KruskalClusterTests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
