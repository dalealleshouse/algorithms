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
#include "matrix_operations.h"
#include "test_helpers.h"

static const size_t kSize = 4096;
// Each value is between 1 and 100 - so that's 3 characters plus 1 for the tab
// Add an additional one to the end for the newline character
static const size_t kBufferSize = 4 * kSize + 1;
static const char* x_matrix =
    "./src/matrix_operations/test_data/random_matrix_1.txt";
static const char* y_matrix =
    "./src/matrix_operations/test_data/random_matrix_2.txt";

static void GenerateMatrixInputFile(const char* file_name) {
  FILE* matrix_file;

  matrix_file = fopen(file_name, "w+");
  if (matrix_file == NULL) {
    perror("Error opening file");
    return;
  }

  for (size_t i = 0; i < kSize; i++) {
    for (size_t j = 0; j < kSize; j++) {
      matrix_value value = random() % 100;
      int result = fprintf(matrix_file, "%ld\t", value);
      if (result < 0) {
        fprintf(stderr, "fprintf error %d\n", result);
        goto done;
      }
    }
    fprintf(matrix_file, "\n");
  }

done:
  fclose(matrix_file);
}

static void GenerateMatrixInputFiles() {
  GenerateMatrixInputFile(x_matrix);
  GenerateMatrixInputFile(y_matrix);
}

static void PrintMatrix(size_t n, matrix_value (*matrix)[n][n]) {
  if (matrix == NULL) {
    fprintf(stderr, "Cannot print a null matrix\n");
    return;
  }

  printf("\n");
  for (size_t i = 0; i < kSize; i++) {
    for (size_t j = 0; j < kSize; j++) {
      printf("%ld ", (*matrix)[i][j]);
    }
    printf("\n");
  }
}

static int ReadMatrixFromFile(const char* path, size_t n,
                              matrix_value (**matrix)[n][n]) {
  if (access(path, R_OK) != 0) {
    fprintf(stderr, "File does not exist or access denied\n");
    return -1;
  }

  ResultCode result_code = Matrix_Initalize(n, matrix);
  if (result_code != kSuccess) {
    fprintf(stderr, "%s\n", Result_ErrorMessage(result_code));
    return -1;
  }

  FILE* matrix_file = fopen(path, "r");
  if (matrix_file == NULL) {
    perror("Error opening file");
    return -1;
  }

  char line[kBufferSize];
  size_t row = 0;
  while (fgets(line, (int)sizeof(line), matrix_file)) {
    char* endptr = line;
    for (size_t i = 0; i < kSize; i++) {
      // if this were a production app, it would be wise to add error checking
      // here...
      matrix_value value = strtol(endptr, &endptr, 10);
      (**matrix)[row][i] = value;
    }
    ++row;
  }

  fclose(matrix_file);
  return 0;
}

static matrix_value SumMatrix(size_t n, matrix_value (*matrix)[n][n]) {
  matrix_value sum = 0;

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      if (__builtin_add_overflow((*matrix)[i][j], sum, &sum)) {
        fprintf(stderr, "%s\n", "Arithmetic Overflow");
        return -1;
      }
    }
  }

  return sum;
}

static void MultiplyLargeMatracies() {
  ResultCode result_code;
  (void)PrintMatrix;
  (void)GenerateMatrixInputFiles;
  typedef matrix_value(*matrix)[kSize][kSize];

  matrix x = NULL;
  matrix y = NULL;
  matrix result = NULL;

  if (ReadMatrixFromFile(x_matrix, kSize, &x) != 0) goto fail;
  if (ReadMatrixFromFile(y_matrix, kSize, &y) != 0) goto fail;

  result_code = Matrix_Initalize(kSize, &result);
  if (result_code != kSuccess) goto result_code_fail;

  result_code = Matrix_StrassenMultiply(kSize, x, y, result);
  if (result_code != kSuccess) goto result_code_fail;

  matrix_value sum = SumMatrix(kSize, result);
  CU_ASSERT_EQUAL(168345695003062, sum);

  return;

result_code_fail:
  PRINT_ERROR("MultiplyLargeMatracies", result_code);
fail:
  Matrices_Destroy(3, x, y, result);
  CU_FAIL();
}

int RegisterMatrixOperationTestCase() {
  CU_TestInfo DisjointSetTests[] = {CU_TEST_INFO(MultiplyLargeMatracies),
                                    CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Matrix Operation Test Case",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = DisjointSetTests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
