#include <stdlib.h>
#include <unistd.h>

#include "CUnit/CUnit.h"
#include "kruskal_cluster.h"
#include "test_helpers.h"
#if !defined(NDEBUG)
#include "malloc_test_wrapper.h"
#endif

static Graph* SmallGraph() {
  return Graph_WeightedFromFile(5,
                                "src/min_spanning_trees/test_data/simple.txt");
}

static Graph* MediumCluster() {
  const static char* PATH = "src/min_spanning_trees/test_data/cluster_med.txt";
  return Graph_WeightedFromFileUndirected(PATH);
}

static Graph* BigCluster() {
  const static char* PATH =
      "src/min_spanning_trees/test_data/cluster_20_128.txt";
  return Graph_WeightedFromFileUndirected(PATH);
}

static Graph* GiantCluster() {
  const static char* PATH = "src/min_spanning_trees/test_data/clustering1.txt";
  return Graph_WeightedFromFileUndirected(PATH);
}

// Common Test Functions
static void KruskalCluster_kNullParameterTest() {
  Result result = KruskalCluster(NULL, 1, NULL);
  CU_ASSERT_EQUAL(result, kNullParameter);
}

static void KruskalCluster_CalcsMaxSpacingForMedCluster() {
  Graph* graph = MediumCluster();

  int max_space;
  Result result = KruskalCluster(graph, 4, &max_space);
  CU_ASSERT_EQUAL(result, kSuccess);
  CU_ASSERT_EQUAL(max_space, 90);

  Graph_Destroy(graph, NULL);
}

static void KruskalCluster_CalcsMaxSpacingForBigCluster() {
  Graph* graph = BigCluster();

  int max_space;
  Result result = KruskalCluster(graph, 4, &max_space);
  CU_ASSERT_EQUAL(result, kSuccess);
  CU_ASSERT_EQUAL(max_space, 578);

  Graph_Destroy(graph, NULL);
}

static void KruskalCluster_CalcsMaxSpacingForGiantCluster() {
  Graph* graph = GiantCluster();

  int max_space;
  Result result = KruskalCluster(graph, 4, &max_space);
  CU_ASSERT_EQUAL(result, kSuccess);
  CU_ASSERT_EQUAL(max_space, 106);

  Graph_Destroy(graph, NULL);
}

static void KruskalCluster_FailedMallocTest() {
#if !defined(NDEBUG)
  Graph* input = SmallGraph();
  Result result;
  int max_space;

  FAILED_MALLOC_TEST({ result = KruskalCluster(input, 2, &max_space); });
  CU_ASSERT_EQUAL(result, kFailedMemoryAllocation);

  Graph_Destroy(input, free);
#endif
}

static void KruskalCluster_InvalidInput() {
  Graph* input = SmallGraph();
  int max_space;

  Result result = KruskalCluster(input, 1, &max_space);
  CU_ASSERT_EQUAL(result, kArgumentOutOfRange);

  Graph_Destroy(input, free);
}

int RegisterKruskalClusterTests() {
  CU_TestInfo KruskalClusterTests[] = {
      CU_TEST_INFO(KruskalCluster_kNullParameterTest),
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
