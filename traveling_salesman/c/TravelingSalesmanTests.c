#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "TravelingSalesman.h"
#include "include/TestHelpers.h"

static const double GRANULARITY = 0.00001;
static const size_t BUFFER_SIZE = 1024;

static void dist_happyPath() {
  double val = 0;

  point p1 = {25, 15};
  point p2 = {35, 10};

  ResultCode result = dist(&p1, &p2, &val);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_DOUBLE_EQUAL(val, 11.18034, GRANULARITY);

  point p_1 = {3, 4};
  point p_2 = {4, 3};

  result = dist(&p_1, &p_2, &val);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_DOUBLE_EQUAL(val, 1.41421, GRANULARITY);
}

static void dist_NullParamter() {
  ResultCode result = dist(NULL, NULL, NULL);
  CU_ASSERT_EQUAL(result, NullParameter);
}

static ResultCode readCitiesFromFile(const char* path, size_t* n,
                                     city** cities) {
  if (path == NULL || cities == NULL) return NullParameter;
  if (*cities != NULL) return OutputPointerIsNotNull;
  if (access(path, R_OK) != 0) return SecurityError;

  FILE* file = fopen(path, "r");
  if (file == NULL) return SystemError;

  // Size is the first line of the file
  char line[BUFFER_SIZE];
  fgets(line, BUFFER_SIZE, file);

  size_t _n = strtoul(line, NULL, 10);
  if (_n == 0) return SystemError;

  city* result = calloc(sizeof(city), _n);
  if (result == NULL) return FailedMemoryAllocation;

  size_t tracker = 0;
  while (fgets(line, BUFFER_SIZE, file)) {
    char* end = NULL;

    // TODO: Add error checking
    double x = strtod(line, &end);
    double y = strtod(end, NULL);

    result[tracker].id = tracker + 1;
    result[tracker].coordinates.x = x;
    result[tracker].coordinates.y = y;
    ++tracker;
  }

  *n = _n;
  *cities = result;
  return Success;
}

static void adjacencyMatrix_NullParamter() {
  ResultCode result = adjacencyMatrix(NULL, 1, NULL);
  CU_ASSERT_EQUAL(result, NullParameter);
}

static void adjacencyMatrix_HappyPath() {
  city* cities = NULL;
  size_t n = 0;

  ResultCode result =
      readCitiesFromFile("src/data/input_float_10_4.txt", &n, &cities);
  CU_ASSERT_EQUAL(result, Success);

  double* graph = NULL;
  result = adjacencyMatrix(cities, n, &graph);
  CU_ASSERT_EQUAL(result, Success);

  free(graph);
  free(cities);
}

static void TravelingSalesman_NullParameter() {
  ResultCode result = TravelingSalesman(10, NULL, NULL);
  CU_ASSERT_EQUAL(result, NullParameter);
}

static void TravelingSalesman_InvalidInput() {
  city* cities = NULL;
  size_t n = 0;
  double val;

  ResultCode result =
      readCitiesFromFile("src/data/input_float_10_4.txt", &n, &cities);
  CU_ASSERT_EQUAL(result, Success);

  double* pGraph = NULL;
  result = adjacencyMatrix(cities, n, &pGraph);
  CU_ASSERT_EQUAL(result, Success);

  double(*graph)[n] = (double(*)[n])pGraph;
  result = TravelingSalesman(0, graph, &val);
  CU_ASSERT_EQUAL(result, ArgumentOutOfRange);

  result = TravelingSalesman(1, graph, &val);
  CU_ASSERT_EQUAL(result, ArgumentOutOfRange);

  free(graph);
  free(cities);
}

static void _testFile(const char* path, const int expected) {
  city* cities = NULL;
  size_t n = 0;
  double* pGraph = NULL;
  double shortest_path = 0;

  ResultCode result = readCitiesFromFile(path, &n, &cities);
  CU_ASSERT_EQUAL(result, Success);

  result = adjacencyMatrix(cities, n, &pGraph);
  CU_ASSERT_EQUAL(result, Success);

  double(*graph)[n] = (double(*)[n])pGraph;

  result = TravelingSalesman(n, graph, &shortest_path);
  CU_ASSERT_EQUAL(result, Success);
  CU_ASSERT_EQUAL(floor(shortest_path), expected);

  free(graph);
  free(cities);
  cities = NULL;
}

static void TravelingSalesman_SolveFile() {
  _testFile("src/data/input_float_10_4.txt", 7);
  _testFile("src/data/input_float_20_6.txt", 12);
  _testFile("src/data/input_float_33_10.txt", 27);
  /* _testFile("src/data/input_float_92_24.txt", 99); */
  /* _testFile("src/data/input_float_25.txt", 26442); */
}

int registerTravelingSalesmanTests() {
  CU_TestInfo TravelingSalesman_Tests[] = {
      CU_TEST_INFO(dist_happyPath),
      CU_TEST_INFO(dist_NullParamter),
      CU_TEST_INFO(adjacencyMatrix_NullParamter),
      CU_TEST_INFO(adjacencyMatrix_HappyPath),
      CU_TEST_INFO(TravelingSalesman_NullParameter),
      CU_TEST_INFO(TravelingSalesman_InvalidInput),
      CU_TEST_INFO(TravelingSalesman_SolveFile),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Traveling Salesman",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = TravelingSalesman_Tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
