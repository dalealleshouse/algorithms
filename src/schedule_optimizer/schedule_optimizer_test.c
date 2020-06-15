#include "./schedule_optimizer.h"

#include <stdlib.h>
#include <unistd.h>

#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

const double EPSILON = 0.0000000000000000001;
const static size_t BUFFER_SIZE = 1024;

// There is minimal error checking b/c this is a constrained use case
static job* _buildScheduleFromFile(char* path) {
  if (access(path, R_OK) != 0) {
    printf("\nFile does not exist or access denied\n");
    return NULL;
  }

  FILE* file = fopen(path, "r");

  // Size is the first line of the file
  char line[BUFFER_SIZE];
  fgets(line, BUFFER_SIZE, file);

  size_t size;
  sscanf(line, "%lu", &size);

  job* jobs = calloc(sizeof(job), size);

  size_t i = 0;
  while (fgets(line, BUFFER_SIZE, file)) {
    sscanf(line, "%lf %lf", &jobs[i].weight, &jobs[i].length);
    ++i;
  }

  fclose(file);
  return jobs;
}

static void ScheduleOptimizer_Calc_null_parameter() {
  double score = 0;
  Result result = ScheduleOptimizer_Calc(NULL, 5, &score);
  CU_ASSERT_EQUAL(kNullParameter, result);

  job jobs = {1.0, 1.0};
  result = ScheduleOptimizer_Calc(&jobs, 1, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result);
}

static void ScheduleOptimizer_Calc_same_lengths() {
  const double expected_score = 275.0;
  job expected[] = {{5, 20}, {5, 10}, {5, 5}};

  double score;
  job jobs[] = {{5, 5}, {5, 10}, {5, 20}};
  size_t n = sizeof(jobs) / sizeof(jobs[0]);

  Result result = ScheduleOptimizer_Calc(jobs, n, &score);
  int cmp_result = memcmp(&jobs, &expected, sizeof(jobs));

  CU_ASSERT_DOUBLE_EQUAL(score, expected_score, EPSILON);
  CU_ASSERT_EQUAL(0, cmp_result);
  CU_ASSERT_EQUAL(kSuccess, result);
}

static void ScheduleOptimizer_Calc_same_weights() {
  const double expected_score = 250.0;
  job expected[] = {{5, 5}, {10, 5}, {15, 5}};

  double score;
  job jobs[] = {{15, 5}, {5, 5}, {10, 5}};
  size_t n = sizeof(jobs) / sizeof(jobs[0]);

  Result result = ScheduleOptimizer_Calc(jobs, n, &score);
  int cmp_result = memcmp(&jobs, &expected, sizeof(jobs));

  CU_ASSERT_DOUBLE_EQUAL(score, expected_score, EPSILON);
  CU_ASSERT_EQUAL(0, cmp_result);
  CU_ASSERT_EQUAL(kSuccess, result);
}

static void ScheduleOptimizer_Calc_empty_array() {
  double score;
  job jobs[1];
  size_t n = 0;

  Result result = ScheduleOptimizer_Calc(jobs, n, &score);
  CU_ASSERT_EQUAL(kEmpty, result);
}

static void ScheduleOptimizer_Calc_large_schedule() {
  double score;
  job* jobs = _buildScheduleFromFile("src/data/jobs.txt");
  size_t n = 10000;

  Result result = ScheduleOptimizer_Calc(jobs, n, &score);
  free(jobs);

  CU_ASSERT_EQUAL(kSuccess, result);
  CU_ASSERT_DOUBLE_EQUAL(score, 67311454237, EPSILON);
}

int register_schedule_optimizer_tests() {
  CU_TestInfo Calc_tests[] = {
      CU_TEST_INFO(ScheduleOptimizer_Calc_null_parameter),
      CU_TEST_INFO(ScheduleOptimizer_Calc_same_lengths),
      CU_TEST_INFO(ScheduleOptimizer_Calc_same_weights),
      CU_TEST_INFO(ScheduleOptimizer_Calc_empty_array),
      CU_TEST_INFO(ScheduleOptimizer_Calc_large_schedule),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "ScheduleOptimizer_Calc",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Calc_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
