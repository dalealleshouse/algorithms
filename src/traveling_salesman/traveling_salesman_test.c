#include "traveling_salesman.h"

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "CUnit/CUnit.h"
#include "test_helpers.h"

static const double GRANULARITY = 0.00001;
static const size_t BUFFER_SIZE = 1024;

static void dist_happyPath() {
  double val = 0;

  point p1 = {25, 15};
  point p2 = {35, 10};

  ResultCode result = dist(&p1, &p2, &val);
  CU_ASSERT_EQUAL(result, kSuccess);
  CU_ASSERT_DOUBLE_EQUAL(val, 11.18034, GRANULARITY);

  point p_1 = {3, 4};
  point p_2 = {4, 3};

  result = dist(&p_1, &p_2, &val);
  CU_ASSERT_EQUAL(result, kSuccess);
  CU_ASSERT_DOUBLE_EQUAL(val, 1.41421, GRANULARITY);
}

static void dist_NullParamter() {
  ResultCode result = dist(NULL, NULL, NULL);
  CU_ASSERT_EQUAL(result, kNullParameter);
}

static ResultCode readCitiesFromFile(const char* path, size_t* n, city** cities,
                                     bool has_ids) {
  if (path == NULL || cities == NULL) return kNullParameter;
  if (*cities != NULL) return kOutputPointerIsNotNull;
  if (access(path, R_OK) != 0) return kSecurityError;

  FILE* file = fopen(path, "r");
  if (file == NULL) return kSystemError;

  // Size is the first line of the file
  char line[BUFFER_SIZE];
  fgets(line, BUFFER_SIZE, file);

  size_t _n = strtoul(line, NULL, 10);
  if (_n == 0) return kSystemError;

  city* result = calloc(sizeof(city), _n);
  if (result == NULL) return kFailedMemoryAllocation;

  size_t tracker = 0;
  while (fgets(line, BUFFER_SIZE, file)) {
    char* end = NULL;

    if (has_ids) {
      size_t _id = strtoul(line, &end, 10);
      if (_id == 0) return kSystemError;
      result[tracker].id = _id;
    } else {
      result[tracker].id = tracker + 1;
      end = line;
    }

    // TODO(dalealleshouse): Add error checking
    double x = strtod(end, &end);
    double y = strtod(end, NULL);

    result[tracker].coordinates.x = x;
    result[tracker].coordinates.y = y;
    result[tracker].visted = false;
    ++tracker;
  }
  result[0].visted = true;

  *n = _n;
  *cities = result;
  return kSuccess;
}

static void adjacencyMatrix_NullParamter() {
  ResultCode result = adjacencyMatrix(NULL, 1, NULL);
  CU_ASSERT_EQUAL(result, kNullParameter);
}

static void adjacencyMatrix_HappyPath() {
  city* cities = NULL;
  size_t n = 0;

  ResultCode result = readCitiesFromFile(
      "src/traveling_salesman/test_data/input_float_10_4.txt", &n, &cities,
      false);
  CU_ASSERT_EQUAL(result, kSuccess);

  double* graph = NULL;
  result = adjacencyMatrix(cities, n, &graph);
  CU_ASSERT_EQUAL(result, kSuccess);

  free(graph);
  free(cities);
}

static void TravelingSalesman_kNullParameter() {
  ResultCode result = TravelingSalesman(10, NULL, NULL);
  CU_ASSERT_EQUAL(result, kNullParameter);
}

static void TravelingSalesman_InvalidInput() {
  city* cities = NULL;
  size_t n = 0;
  double val;

  ResultCode result = readCitiesFromFile(
      "src/traveling_salesman/test_data/input_float_10_4.txt", &n, &cities,
      false);
  CU_ASSERT_EQUAL(result, kSuccess);

  double* pGraph = NULL;
  result = adjacencyMatrix(cities, n, &pGraph);
  CU_ASSERT_EQUAL(result, kSuccess);

  double(*graph)[n] = (double(*)[n])pGraph;
  result = TravelingSalesman(0, graph, &val);
  CU_ASSERT_EQUAL(result, kArgumentOutOfRange);

  result = TravelingSalesman(1, graph, &val);
  CU_ASSERT_EQUAL(result, kArgumentOutOfRange);

  free(graph);
  free(cities);
}

static void _testFile(const char* path, const int expected) {
  city* cities = NULL;
  size_t n = 0;
  double* pGraph = NULL;
  double shortest_path = 0;

  ResultCode result = readCitiesFromFile(path, &n, &cities, false);
  CU_ASSERT_EQUAL(result, kSuccess);

  result = adjacencyMatrix(cities, n, &pGraph);
  CU_ASSERT_EQUAL(result, kSuccess);

  double(*graph)[n] = (double(*)[n])pGraph;

  result = TravelingSalesman(n, graph, &shortest_path);
  CU_ASSERT_EQUAL(result, kSuccess);
  CU_ASSERT_EQUAL(floor(shortest_path), expected);

  free(graph);
  free(cities);
  cities = NULL;
}

static void TravelingSalesman_SolveFile() {
  _testFile("src/traveling_salesman/test_data/input_float_10_4.txt", 7);
  _testFile("src/traveling_salesman/test_data/input_float_20_6.txt", 12);
  _testFile("src/traveling_salesman/test_data/input_float_33_10.txt", 27);
  /* _testFile("src/traveling_salesman/test_data/input_float_92_24.txt", 99); */
  /* _testFile("src/traveling_salesman/test_data/input_float_25.txt", 26442); */
}

static void _testFileApprox(const char* path, const int expected) {
  city* cities = NULL;
  size_t n = 0;
  double shortest_path = 0;

  ResultCode result = readCitiesFromFile(path, &n, &cities, true);
  CU_ASSERT_EQUAL(result, kSuccess);

  result = TravelingSalesmanApprox(n, cities, &shortest_path);
  CU_ASSERT_EQUAL(result, kSuccess);
  CU_ASSERT_EQUAL(floor(shortest_path), expected);

  free(cities);
  cities = NULL;
}

static void TravelingSalesmanApprox_SolveFile() {
  _testFileApprox("src/traveling_salesman/test_data/input_simple_10_8.txt", 23);
  _testFileApprox("src/traveling_salesman/test_data/input_simple_20_20.txt",
                  74);
  _testFileApprox("src/traveling_salesman/test_data/input_simple_37_400.txt",
                  7520);
  /* _testFileApprox("src/traveling_salesman/test_data/input_simple_68_20000.txt",
   * 2506548); */
  /* _testFileApprox("src/traveling_salesman/test_data/input_simple_nn.txt",
   * 1203406); */
}

int RegisterTravelingSalesmanTests() {
  CU_TestInfo TravelingSalesman_Tests[] = {
      CU_TEST_INFO(dist_happyPath),
      CU_TEST_INFO(dist_NullParamter),
      CU_TEST_INFO(adjacencyMatrix_NullParamter),
      CU_TEST_INFO(adjacencyMatrix_HappyPath),
      CU_TEST_INFO(TravelingSalesman_kNullParameter),
      CU_TEST_INFO(TravelingSalesman_InvalidInput),
      CU_TEST_INFO(TravelingSalesman_SolveFile),
      CU_TEST_INFO(TravelingSalesmanApprox_SolveFile),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Traveling Salesman",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = TravelingSalesman_Tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
