#include "./knapsack_problem.h"

#include <stdlib.h>
#include <unistd.h>

#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

static void _containsId(Item** items, size_t n, id id) {
  for (size_t i = 0; i < n; i++)
    if (items[i]->id == id) return;

  CU_FAIL("Item id not found");
}

static Knapsack* _smallKnapsack() {
  const size capacity = 6;
  const size_t n = 4;
  id ids[] = {1, 2, 3, 4};
  value values[] = {3, 2, 4, 4};
  size sizes[] = {4, 3, 2, 3};
  Knapsack* problem = NULL;

  ResultCode code = Knapsack_Init(ids, values, sizes, n, capacity, &problem);
  if (code != Success) {
    printf("/n%s/.", Result_ErrorMessage(code));
    CU_FAIL("Failed to intalize the small knapsack");
  }

  return problem;
}

static void Knapsack_Init_NullParameter() {
  ResultCode code = Knapsack_Init(NULL, NULL, NULL, 5, 0, NULL);
  CU_ASSERT_EQUAL(code, NullParameter);
}

static void Knapsack_Init_InvalidParameters() {
  id ids[] = {1};
  value values[] = {1};
  size sizes[] = {1};
  Knapsack* solution = NULL;

  // invalid n
  ResultCode code = Knapsack_Init(ids, values, sizes, 0, 5, &solution);
  CU_ASSERT_EQUAL(code, ArgumentOutOfRange);

  // invalid size
  code = Knapsack_Init(ids, values, sizes, 1, 0, &solution);
  CU_ASSERT_EQUAL(code, ArgumentOutOfRange);

  // invalid pointer
  Knapsack* initalized = malloc(sizeof(Knapsack));
  code = Knapsack_Init(ids, values, sizes, 1, 5, &initalized);
  CU_ASSERT_EQUAL(code, OutputPointerIsNotNull);

  free(initalized);
}

static void Knapsack_Init_HappyPath() {
  const size capacity = 5;
  const size_t n = 2;

  id ids[] = {1, 2};
  value values[] = {1, 2};
  size sizes[] = {1, 2};
  Knapsack* problem = NULL;

  ResultCode code = Knapsack_Init(ids, values, sizes, n, capacity, &problem);

  CU_ASSERT_EQUAL(code, Success);
  CU_ASSERT_EQUAL(problem->capacity, capacity);
  CU_ASSERT_EQUAL(problem->n, n);

  for (size_t i = 0; i < n; i++) {
    CU_ASSERT_EQUAL(problem->items[i]->id, ids[i]);
    CU_ASSERT_EQUAL(problem->items[i]->value, values[i]);
    CU_ASSERT_EQUAL(problem->items[i]->size, sizes[i]);
  }

  Knapsack_Destory(problem);
}

static void Knapsack_Pack_NullParameter() {
  ResultCode code = Knapsack_Pack(NULL, NULL);
  CU_ASSERT_EQUAL(code, NullParameter);
}

static void Knapsack_Pack_InvalidOutputPointer() {
  Knapsack* small = _smallKnapsack();
  ResultCode code = Knapsack_Pack(small, NULL);
  CU_ASSERT_EQUAL(code, NullParameter);

  PackedKnapsack* temp = malloc(sizeof(PackedKnapsack));
  code = Knapsack_Pack(small, &temp);
  CU_ASSERT_EQUAL(code, OutputPointerIsNotNull);

  Knapsack_Destory(small);
  free(temp);
}

static void Knapsack_Pack_HappyPath() {
  Knapsack* small = _smallKnapsack();
  PackedKnapsack* solution = NULL;

  ResultCode code = Knapsack_Pack(small, &solution);

  CU_ASSERT_EQUAL(code, Success);
  CU_ASSERT_EQUAL(solution->orginal_capacity, small->capacity);
  CU_ASSERT_EQUAL(solution->n, 2);
  CU_ASSERT_EQUAL(solution->used_capacity, 5);
  CU_ASSERT_EQUAL(solution->total_value, 8);
  _containsId(solution->items, solution->n, 3);
  _containsId(solution->items, solution->n, 4);

  PackedKnapsack_Destory(solution);
  Knapsack_Destory(small);
}

static Knapsack* _readFile(const char* path) {
  const static int BUFFER_SIZE = 1024;

  if (access(path, R_OK) != 0) {
    printf("File does not exist or access denied\n");
    return NULL;
  }

  FILE* file = fopen(path, "r");

  char line[BUFFER_SIZE];
  char* remaining;
  fgets(line, BUFFER_SIZE, file);

  size capacity = strtoul(line, &remaining, 10);
  if (capacity == 0) {
    printf("Failed size_t conversion\n");
    return NULL;
  }

  size_t n = strtoul(remaining, NULL, 10);
  if (n == 0) {
    printf("Failed size_t conversion\n");
    return NULL;
  }

  id ids[n];
  size sizes[n];
  value values[n];

  size_t curr_id = 1;
  while (fgets(line, BUFFER_SIZE, file)) {
    size_t index = curr_id - 1;
    values[index] = strtol(line, &remaining, 10);
    sizes[index] = strtol(remaining, NULL, 10);
    ids[index] = curr_id;
    ++curr_id;
  }

  Knapsack* knapsack = NULL;
  Knapsack_Init(ids, values, sizes, n, capacity, &knapsack);

  fclose(file);
  return knapsack;
}

static void _testFile(const char* path, const value expected_value) {
  Knapsack* knapsack = _readFile(path);
  value solution = 0;

  ResultCode code = Knapsack_Pack_Optimal(knapsack, &solution);
  CU_ASSERT_EQUAL(code, Success);
  CU_ASSERT_EQUAL(solution, expected_value);

  Knapsack_Destory(knapsack);
}

static void Knapsack_SolveFiles() {
  _testFile("src/knapsack_problem/test_data/knapsack1.txt", 2493893);
  /* _testFile("src/knapsack_problem/test_data/knapsack_big.txt", 4243395); */
  _testFile("src/knapsack_problem/test_data/input_random_11_100_10.txt", 210);
  _testFile("src/knapsack_problem/test_data/input_random_16_100_100.txt", 539);
  /* _testFile("src/knapsack_problem/test_data/input_random_44_2000000_2000.txt",
   * 49957183); */
}

int RegisterKnapsackProblemTests() {
  CU_TestInfo KnapsackProblem_Tests[] = {
      CU_TEST_INFO(Knapsack_Init_NullParameter),
      CU_TEST_INFO(Knapsack_Init_InvalidParameters),
      CU_TEST_INFO(Knapsack_Init_HappyPath),
      CU_TEST_INFO(Knapsack_Pack_NullParameter),
      CU_TEST_INFO(Knapsack_Pack_InvalidOutputPointer),
      CU_TEST_INFO(Knapsack_Pack_HappyPath),
      CU_TEST_INFO(Knapsack_SolveFiles),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Knapsack Problem",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = KnapsackProblem_Tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
