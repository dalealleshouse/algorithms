#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

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

static void _testNullParameter(wis func) {
  Result result = func(NULL, NULL);
  CU_ASSERT_EQUAL(result, NullParameter);
}

static void _testEmptySet(wis func) {
  WeightedIndependentSet* wis = NULL;
  PathGraph graph;
  graph.n = 0;
  graph.vertices = NULL;

  Result result = func(&graph, &wis);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_PTR_EQUAL(wis, &EMPTY_SET);
}

static void _testSingleVertex(wis func) {
  unsigned int ids[] = {1};
  unsigned long weights[] = {100};
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

static void _testTwoVertices(wis func) {
  unsigned int ids[] = {1, 2};
  unsigned long weights[] = {100, 101};
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

static void _testFourVertices(wis func) {
  unsigned int ids[] = {0, 1, 2, 3};
  unsigned long weights[] = {1, 4, 5, 4};
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

static void _testSixVertices(wis func) {
  unsigned int ids[] = {0, 1, 2, 3, 4, 5};
  unsigned long weights[] = {3, 2, 1, 6, 4, 5};
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

static void _testArimiticOverflow(wis func) {
  unsigned int ids[] = {0, 1, 2, 3};
  unsigned long weights[] = {1, ULONG_MAX, 5, 4};
  PathGraph* graph;
  PathGraph_Create(ids, weights, 4, &graph);
  WeightedIndependentSet* wis = NULL;

  Result result = func(graph, &wis);
  CU_ASSERT_EQUAL(result, ArithmeticOverflow);

  WeightedIndependentSet_Destroy(wis);
  PathGraph_Destroy(graph);
}

static void WeightedIndependentSet_Recursive_NullParameter() {
  _testNullParameter(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_EmptySet() {
  _testEmptySet(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_SingleVertex() {
  _testSingleVertex(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_TwoVertices() {
  _testTwoVertices(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_FourVertices() {
  _testFourVertices(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_SixVertices() {
  _testSixVertices(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Recursive_ArimiticOverflow() {
  _testArimiticOverflow(WeightedIndependentSet_Recursive);
}

static void WeightedIndependentSet_Dynamic_NullParameter() {
  _testNullParameter(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_EmptySet() {
  _testEmptySet(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_SingleVertex() {
  _testSingleVertex(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_TwoVertices() {
  _testTwoVertices(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_FourVertices() {
  _testFourVertices(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_SixVertices() {
  _testSixVertices(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_ArimiticOverflow() {
  _testArimiticOverflow(WeightedIndependentSet_Dynamic);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_NullParameter() {
  _testNullParameter(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_EmptySet() {
  _testEmptySet(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_SingleVertex() {
  _testSingleVertex(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_TwoVertices() {
  _testTwoVertices(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_FourVertices() {
  _testFourVertices(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_SixVertices() {
  _testSixVertices(WeightedIndependentSet_Dynamic_Reconstruction);
}

static void WeightedIndependentSet_Dynamic_Reconstruction_ArimiticOverflow() {
  _testArimiticOverflow(WeightedIndependentSet_Dynamic_Reconstruction);
}

static PathGraph* _readFile(const char* path) {
  const static int BUFFER_SIZE = 1024;

  if (access(path, R_OK) != 0) {
    printf("File does not exist or access denied\n");
    return NULL;
  }

  FILE* file = fopen(path, "r");

  // Size is the first line of the file
  char line[BUFFER_SIZE];
  fgets(line, BUFFER_SIZE, file);

  size_t n = strtoul(line, NULL, 10);
  if (n == 0) {
    printf("Failed size_t conversion\n");
    return NULL;
  }

  PathGraph* graph = malloc(sizeof(PathGraph));
  graph->n = n;
  graph->vertices = calloc(sizeof(void*), n);

  size_t weight = 0;
  size_t curr_id = 1;
  while (fgets(line, BUFFER_SIZE, file)) {
    weight = strtol(line, NULL, 10);
    WeightedVertex_Init(curr_id, weight, &graph->vertices[curr_id - 1]);
    ++curr_id;
  }

  fclose(file);
  return graph;
}

static void _testFile(const char* path, const char* solution) {
  WeightedIndependentSet* wis = NULL;
  PathGraph* graph = _readFile(path);

  Result result = WeightedIndependentSet_Dynamic_Reconstruction(graph, &wis);
  CU_ASSERT_EQUAL(result, Success);

  char _solution[] = "00000000\0";

  // 1, 2, 3, 4, 17, 117, 517, 997
  // You could defintly improve on the run time of this, but it's not that
  // important because it's test code
  for (size_t i = 0; i < wis->n; i++) {
    switch (wis->vertices[i]->id) {
      case 1:
        _solution[0] = '1';
        break;
      case 2:
        _solution[1] = '1';
        break;
      case 3:
        _solution[2] = '1';
        break;
      case 4:
        _solution[3] = '1';
        break;
      case 17:
        _solution[4] = '1';
        break;
      case 117:
        _solution[5] = '1';
        break;
      case 517:
        _solution[6] = '1';
        break;
      case 997:
        _solution[7] = '1';
        break;
    }
  }

  /* printf("solution=%s, my_solution=%s\n", solution, _solution); */
  CU_ASSERT_STRING_EQUAL(_solution, solution);

  WeightedIndependentSet_Destroy(wis);
  PathGraph_Destroy(graph);
}

static void WeightedIndependentSet_File() {
  _testFile("src/data/input_random_10_40.txt", "10010000");
  _testFile("src/data/input_random_48_10000.txt", "10101111");
  _testFile("src/data/mwis.txt", "10100110");
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

  CU_TestInfo WeightedIndependentSet_Files[] = {
      CU_TEST_INFO(WeightedIndependentSet_File), CU_TEST_INFO_NULL};

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
      {.pName = "File Solutions",
       .pInitFunc = noop,
       .pCleanupFunc = noop,
       .pTests = WeightedIndependentSet_Files},
      CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
