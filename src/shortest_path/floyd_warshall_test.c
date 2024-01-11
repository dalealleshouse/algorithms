#include "floyd_warshall.h"

#include <limits.h>
#include <stdlib.h>

#include "CUnit/CUnit.h"
#include "bellman_ford.h"
#include "test_helpers.h"

const static size_t small_n = 6;
const static char* small_path =
    "src/shortest_path/test_data/small_negative_weight.txt";

const static size_t neg_n = 6;
const static char* neg_path =
    "src/shortest_path/test_data/small_negative_cycle.txt";

static void _testFile(const char* path, const int expected, const size_t path_n,
                      const vertex_id expected_path[path_n]) {
  Graph* sut = Graph_WeightedFromFileDirected(path);

  // Find the smallest path length
  path_length* solutions;
  GraphResult result = FloydWarshallShortestPath(sut, &solutions);
  CU_ASSERT_EQUAL(result, Graph_kSuccess);

  int shortest = UNINITIALIZED;
  for (size_t v = 0; v < sut->n; v++) {
    for (size_t w = 0; w < sut->n; w++) {
      if (solutions[v + sut->n * w] < shortest) {
        shortest = solutions[v + sut->n * w];
      }
    }
  }

  CU_ASSERT_EQUAL(shortest, expected);

  // Validate the path for the shortest length
  Path* actual_path = NULL;
  result = BellmanFordShortestPath(sut, expected_path[0]);
  CU_ASSERT_EQUAL(result, Graph_kSuccess);

  result = BellmandFordTracePath(sut, expected_path[path_n - 1], &actual_path);
  CU_ASSERT_EQUAL(result, Graph_kSuccess);

  size_t tracker = path_n;
  Path* p = actual_path;

  while (tracker > 0) {
    CU_ASSERT_EQUAL(tracker, tracker);
    CU_ASSERT_EQUAL(p->vertex, expected_path[path_n - tracker]);

    p = p->next;
    tracker--;
  }

  free(solutions);
  Path_Destroy(actual_path);
  Graph_Destroy(sut, free);
}

static void FloydWarshallShortestPath_NullParamter() {
  GraphResult result = FloydWarshallShortestPath(NULL, NULL);
  CU_ASSERT_EQUAL(result, Graph_kNullParameter);
}

static void FloydWarshallShortestPath_HappyPath() {
  Graph* sut = Graph_WeightedFromFile(small_n, small_path);

  path_length* solutions = NULL;
  GraphResult result = FloydWarshallShortestPath(sut, &solutions);
  CU_ASSERT_EQUAL(result, Graph_kSuccess);

  CU_ASSERT_EQUAL(UNINITIALIZED, solutions[1 + small_n * 0]);
  CU_ASSERT_EQUAL(0, solutions[1 + small_n * 1]);
  CU_ASSERT_EQUAL(1, solutions[2 + small_n * 1]);
  CU_ASSERT_EQUAL(5, solutions[3 + small_n * 1]);
  CU_ASSERT_EQUAL(4, solutions[4 + small_n * 1]);
  CU_ASSERT_EQUAL(2, solutions[5 + small_n * 1]);

  free(solutions);
  Graph_Destroy(sut, free);
}

static void FloydWarshallShortestPath_kArithmeticOverflow() {
  Graph* sut = Graph_WeightedFromFile(small_n, small_path);

  sut->V[3]->in_edges->weight = INT_MAX - 1;

  path_length* solutions = NULL;
  GraphResult result = FloydWarshallShortestPath(sut, &solutions);
  CU_ASSERT_EQUAL(result, Graph_kArithmeticOverflow);

  Graph_Destroy(sut, free);
}

static void _testNegativeCycle(Graph* sut) {
  path_length* solutions = NULL;

  GraphResult result = FloydWarshallShortestPath(sut, &solutions);
  CU_ASSERT_EQUAL(result, Graph_NegativeCycle);

  free(solutions);
  Graph_Destroy(sut, NULL);
}

static void FloydWarshallShortestPath_NegativeCycle() {
  _testNegativeCycle(Graph_WeightedFromFile(neg_n, neg_path));
  _testNegativeCycle(
      Graph_WeightedFromFileDirected("src/shortest_path/test_data/17_32.txt"));
  // Slow tests, no need to execute everytime
  /* _testNegativeCycle(Graph_WeightedFromFileDirected("src/graphs/g1.txt")); */
  /* _testNegativeCycle(Graph_WeightedFromFileDirected("src/graphs/g2.txt")); */
}

static void FloydWarshallShortestPath_Files() {
  const vertex_id expected[] = {1, 7};
  _testFile("src/shortest_path/test_data/10_8.txt", -41, 2, expected);

  const vertex_id expected2[] = {2, 5};
  _testFile("src/shortest_path/test_data/11_8.txt", -89, 2, expected2);

  const vertex_id expected3[] = {1, 12, 15};
  _testFile("src/shortest_path/test_data/14_16.txt", -130, 3, expected3);

  const vertex_id expected4[] = {4, 5, 9, 11, 16};
  _testFile("src/shortest_path/test_data/16_16.txt", -191, 5, expected4);

  const vertex_id expected5[] = {4, 7, 13, 17, 23, 28, 30};
  _testFile("src/shortest_path/test_data/18_32.txt", -355, 7, expected5);

  const vertex_id expected6[] = {2, 20, 21, 26};
  _testFile("src/shortest_path/test_data/19_32.txt", -280, 4, expected6);

  const vertex_id expected7[] = {4,   15,  34,  47,  53,  68,  70,  73,  87,
                                 103, 124, 155, 172, 188, 225, 227, 236, 252};
  _testFile("src/shortest_path/test_data/30_256.txt", -961, 18, expected7);

  const vertex_id expected8[] = {21,  24,  58,  86,  102, 118, 127,
                                 173, 189, 192, 197, 242, 251};
  _testFile("src/shortest_path/test_data/32_256.txt", -778, 13, expected8);

  // Slow tests, no need to execute everytime
  /* const vertex_id expected9[] = { */
  /*     7,    44,   78,   91,   140,  173,  239,  291,  294,  324,  370, */
  /*     376,  449,  580,  605,  632,  640,  664,  737,  806,  825,  848, */
  /*     892,  910,  911,  914,  976,  1026, 1040, 1068, 1157, 1189, 1204, */
  /*     1241, 1242, 1253, 1291, 1317, 1387, 1392, 1430, 1537, 1567, 1581, */
  /*     1600, 1612, 1635, 1691, 1709, 1783, 1788, 1828, 1836, 1927, 1930, */
  /*     1946, 1966, 1980, 1986, 1989, 2006, 2008, 2044}; */
  /* _testFile("src/graphs/44_2048.txt", -3127, 63, expected9); */
}

int RegisterFloydWarshallTests() {
  CU_TestInfo tests[] = {
      CU_TEST_INFO(FloydWarshallShortestPath_NullParamter),
      CU_TEST_INFO(FloydWarshallShortestPath_HappyPath),
      CU_TEST_INFO(FloydWarshallShortestPath_kArithmeticOverflow),
      CU_TEST_INFO(FloydWarshallShortestPath_NegativeCycle),
      CU_TEST_INFO(FloydWarshallShortestPath_Files),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Floyd Warshall",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
