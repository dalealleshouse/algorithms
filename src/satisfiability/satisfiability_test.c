#include "satisfiability.h"

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "../utils/test_helpers.h"
#include "CUnit/CUnit.h"

static const size_t BUFFER_SIZE = 1024;

static ResultCode _importFile(const char* path, clause** clauses, size_t* n) {
  if (path == NULL) return kNullParameter;
  if (*clauses != NULL) return kOutputPointerIsNotNull;
  if (access(path, R_OK) != 0) return kSecurityError;

  FILE* file = fopen(path, "r");
  if (file == NULL) return kSystemError;

  // Size is the first line of the file
  char line[BUFFER_SIZE];
  fgets(line, BUFFER_SIZE, file);

  size_t _n = strtoul(line, NULL, 10);
  if (_n == 0) return kSystemError;

  clause* result = calloc(sizeof(clause), _n);
  if (result == NULL) return kFailedMemoryAllocation;

  size_t tracker = 0;
  while (fgets(line, BUFFER_SIZE, file)) {
    char* end = NULL;

    /*   // TODO: Add error checking */
    result[tracker].a = strtol(line, &end, 10);
    result[tracker].b = strtol(end, NULL, 10);

    ++tracker;
  }

  *n = _n;
  *clauses = result;
  return kSuccess;
}

static void _testFile(const char* path, bool expected) {
  clause* clauses = NULL;
  size_t n = 0;

  ResultCode result = _importFile(path, &clauses, &n);
  CU_ASSERT_EQUAL(result, kSuccess);

  bool satisfiable = false;
  result = Sat_Solve(n, clauses, &satisfiable);
  CU_ASSERT_EQUAL(result, kSuccess);

  CU_ASSERT_EQUAL(satisfiable, expected);
  free(clauses);
}

static void Sat_EvalClause_NullParamter() {
  ResultCode result = Sat_EvalClause(NULL, true, true, NULL);
  CU_ASSERT_EQUAL(result, kNullParameter);
}

static void _testSatClause(clause* clause, bool a, bool b, bool expected) {
  bool satisfied = false;
  ResultCode result = Sat_EvalClause(clause, a, b, &satisfied);

  CU_ASSERT_EQUAL(result, kSuccess);
  CU_ASSERT_EQUAL(satisfied, expected);
}

static void Sat_EvalClause_TrueWhenSatisfied() {
  clause c1 = {1, 2};
  _testSatClause(&c1, true, false, true);

  clause c2 = {-1, 3};
  _testSatClause(&c2, true, true, true);

  clause c3 = {3, 4};
  _testSatClause(&c3, true, false, true);

  clause c4 = {-2, -4};
  _testSatClause(&c4, false, false, true);
}

static void Sat_EvalClause_FalseWhenNotSatisfied() {
  clause c1 = {1, 2};
  _testSatClause(&c1, false, false, false);

  clause c2 = {-1, 3};
  _testSatClause(&c2, true, false, false);

  clause c3 = {3, 4};
  _testSatClause(&c3, false, false, false);

  clause c4 = {-2, -4};
  _testSatClause(&c4, true, true, false);
}

static void Sat_SolveFiles() {
  /*
   * 10_20 = 0
   * 11_40 = 1
   * 13_80 = 1
   * 16_100 = 1
   * 17_200 = 1
   * 28_4000 = 0
   * 31_10000 = 1
   */
  _testFile("src/satisfiability/test_data/input_beaunus_10_20.txt", 0);
}

int RegisterSatisfiabilityTests() {
  CU_TestInfo Satisfiability_Tests[] = {
      CU_TEST_INFO(Sat_EvalClause_NullParamter),
      CU_TEST_INFO(Sat_EvalClause_TrueWhenSatisfied),
      CU_TEST_INFO(Sat_EvalClause_FalseWhenNotSatisfied),

      CU_TEST_INFO(Sat_SolveFiles), CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Satisfiability",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Satisfiability_Tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
