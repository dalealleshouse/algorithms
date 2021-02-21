/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include <stdlib.h>
#include <unistd.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "running_median.h"
#include "test_helpers.h"

static const size_t kSize = 100000;
static const size_t kBufferSize = 256;
static const char* kFileName =
    "src/running_median/test_data/random_floating_points.txt";

static void GenerateRandomFloatingPointFile(const char* file_name) {
  FILE* file;

  file = fopen(file_name, "w+");
  if (file == NULL) {
    perror("Error opening file");
    return;
  }

  for (size_t i = 0; i < kSize; i++) {
    median_value value = drand48() * 100;

    int result = fprintf(file, "%f\n", value);
    if (result < 0) {
      fprintf(stderr, "fprintf error %d\n", result);
      goto done;
    }
  }

done:
  fclose(file);
}

static int ReadValuesFromFile(const char* path, size_t n,
                              median_value (**result)[n]) {
  if (access(path, R_OK) != 0) {
    fprintf(stderr, "File does not exist or access denied\n");
    return -1;
  }

  FILE* file = fopen(path, "r");
  if (file == NULL) {
    perror("Error opening file");
    return -1;
  }

  // clang-analyzer-unix.MallocSizeof - I think this is a problem due to the
  // variable length array
  // NOLINTNEXTLINE
  *result = calloc(n, sizeof(median_value));
  if (*result == NULL) {
    perror("Failed malloc");
    return -1;
  }

  char line[kBufferSize];
  size_t i = 0;
  while (fgets(line, (int)sizeof(line), file)) {
    char* endptr = line;
    median_value value = strtod(endptr, &endptr);
    (**result)[i++] = value;
  }

  fclose(file);
  return 0;
}

static median_value GetSumOfMedians(size_t sliding_window) {
  median_value sum = 0;
  ResultCode result_code = kSuccess;
  RunningMedian* rm = NULL;
  median_value(*values)[kSize] = NULL;

  result_code = RunningMedian_Create(&rm, sliding_window);
  if (result_code != kSuccess) goto fail;

  if (ReadValuesFromFile(kFileName, kSize, &values) != 0) {
    result_code = kDependancyError;
    goto fail;
  }

  for (size_t i = 0; i < kSize; i++) {
    result_code = RunningMedian_Insert(rm, (*values)[i]);
    if (result_code != kSuccess) goto fail;

    median_value median;
    result_code = RunningMedian_Median(rm, &median);
    if (result_code != kSuccess) goto fail;

    sum += median;
  }

fail:
  RunningMedian_Destroy(rm);
  free(values);

  if (result_code != kSuccess) {
    PRINT_ERROR("SumOfMedians", result_code);
    return -1;
  }

  return sum;
}

static void SumOfMedians() {
  (void)GenerateRandomFloatingPointFile;
  /* GenerateRandomFloatingPointFile(kFileName); */

  /* median_value sum = GetSumOfMedians(0); */
  /* printf("\nThe sum of all median values = %f\n", sum); */

  median_value sum = GetSumOfMedians(100);
  printf("\nThe sum of all median values with a sliding window = %f\n", sum);
}

int RegisterRunningMedianTestCase() {
  CU_TestInfo DisjointSetTests[] = {CU_TEST_INFO(SumOfMedians),
                                    CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Running Median Test Case",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = DisjointSetTests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
