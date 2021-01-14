/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include <stdlib.h>

#include "CUnit/CUnit.h"
#include "malloc_test_wrapper.h"
#include "matrix_operations.h"
#include "test_helpers.h"

#define SUT(N, code_block)                            \
  {                                                   \
    const size_t n = N;                               \
                                                      \
    matrix_value(*x)[n][n] = NULL;                    \
    matrix_value(*y)[n][n] = NULL;                    \
    matrix_value(*z)[n][n] = NULL;                    \
                                                      \
    ResultCode result_code = Matrix_Initalize(n, &x); \
    CU_ASSERT_EQUAL(kSuccess, result_code);           \
                                                      \
    result_code = Matrix_Initalize(n, &y);            \
    CU_ASSERT_EQUAL(kSuccess, result_code);           \
                                                      \
    result_code = Matrix_Initalize(n, &z);            \
    CU_ASSERT_EQUAL(kSuccess, result_code);           \
                                                      \
    {code_block}                                      \
                                                      \
    free(x);                                          \
    free(y);                                          \
    free(z);                                          \
  }

static void Matrix_Initalize_failed_malloc() {
#if !defined(NDEBUG)
  FAILED_MALLOC_TEST({
    const size_t n = 10;
    matrix_value(*result)[n][n] = NULL;
    ResultCode result_code = Matrix_Initalize(n, &result);
    CU_ASSERT_EQUAL(kFailedMemoryAllocation, result_code);
  });
#endif
}

static void Matrix_Initalize_invalid_size() {
  const size_t n = 0;
  matrix_value(*result)[n][n] = NULL;
  ResultCode result_code = Matrix_Initalize(n, &result);
  CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
}

static void Matrix_Initalize_null_parameter() {
  ResultCode result_code = Matrix_Initalize(10, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);
}

static void Matrix_Initalize_non_null_output_parameter() {
  const size_t n = 10;
  matrix_value(*result)[n][n] = malloc(1);

  ResultCode result_code = Matrix_Initalize(n, &result);
  CU_ASSERT_EQUAL(kOutputPointerIsNotNull, result_code);

  free(result);
}

static void Matrix_Initalize_size_causes_overflow() {
  matrix_value(*result)[1][1] = NULL;
  ResultCode result_code = Matrix_Initalize(SIZE_MAX, &result);
  CU_ASSERT_EQUAL(kArithmeticOverflow, result_code);
}

static void Matrix_Initalize_initalizes_to_zero() {
  const size_t n = 10;
  matrix_value(*matrix)[n][n] = NULL;
  ResultCode result_code = Matrix_Initalize(n, &matrix);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  if (matrix == NULL) {
    CU_FAIL("matrix is null");
    return;
  }

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      CU_ASSERT_EQUAL(0, (*matrix)[i][j]);
    }
  }

  free(matrix);
}

static void Matrix_Add_null_parameters() {
  const size_t n = 2;

  matrix_value(*x)[n][n] = NULL;
  matrix_value(*y)[n][n] = NULL;

  ResultCode result_code = Matrix_Initalize(n, &x);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_Initalize(n, &y);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_Add(n, x, y, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Matrix_Add(n, x, NULL, y);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Matrix_Add(n, NULL, x, y);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  free(x);
  free(y);
}

static void Matrix_Add_invalid_size() {
  SUT(2, {
    result_code = Matrix_Add(0, x, y, z);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
  });
}

static void Matrix_Add_happy_path() {
  SUT(2, {
    matrix_value(*expected)[n][n] = NULL;

    ResultCode result_code = Matrix_Initalize(n, &expected);
    CU_ASSERT_EQUAL(kSuccess, result_code);

    (*expected)[0][0] = 5;
    (*expected)[0][1] = 8;
    (*expected)[1][0] = 8;
    (*expected)[1][1] = 9;

    (*x)[0][0] = 4;
    (*x)[0][1] = 8;
    (*x)[1][0] = 3;
    (*x)[1][1] = 7;

    (*y)[0][0] = 1;
    (*y)[0][1] = 0;
    (*y)[1][0] = 5;
    (*y)[1][1] = 2;

    result_code = Matrix_Add(2, x, y, z);
    CU_ASSERT_EQUAL(kSuccess, result_code);
    CU_ASSERT_EQUAL(0, memcmp(z, expected, sizeof(*expected)));

    free(expected);
  });
}

static void Matrix_Add_arithmetic_overflow() {
  SUT(2, {
    (*x)[0][0] = INT64_MAX;
    (*y)[0][0] = 1;

    result_code = Matrix_Add(n, x, y, z);
    CU_ASSERT_EQUAL(kArithmeticOverflow, result_code);
  });
}

static void Matrix_Subtract_null_parameters() {
  const size_t n = 2;

  matrix_value(*x)[n][n] = NULL;
  matrix_value(*y)[n][n] = NULL;

  ResultCode result_code = Matrix_Initalize(n, &x);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_Initalize(n, &y);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_Subtract(n, x, y, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Matrix_Subtract(n, x, NULL, y);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Matrix_Subtract(n, NULL, x, y);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  free(x);
  free(y);
}

static void Matrix_Subtract_invalid_size() {
  SUT(2, {
    result_code = Matrix_Subtract(0, x, y, z);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
  });
}

static void Matrix_Subtract_happy_path() {
  SUT(2, {
    matrix_value(*expected)[n][n] = NULL;

    ResultCode result_code = Matrix_Initalize(n, &expected);
    CU_ASSERT_EQUAL(kSuccess, result_code);

    (*expected)[0][0] = -3;
    (*expected)[0][1] = 2;
    (*expected)[1][0] = -11;
    (*expected)[1][1] = 6;

    (*x)[0][0] = 2;
    (*x)[0][1] = 8;
    (*x)[1][0] = 0;
    (*x)[1][1] = 9;

    (*y)[0][0] = 5;
    (*y)[0][1] = 6;
    (*y)[1][0] = 11;
    (*y)[1][1] = 3;

    result_code = Matrix_Subtract(2, x, y, z);
    CU_ASSERT_EQUAL(kSuccess, result_code);
    CU_ASSERT_EQUAL(0, memcmp(z, expected, sizeof(*expected)));

    free(expected);
  });
}

static void Matrix_Subtract_arithmetic_overflow() {
  SUT(2, {
    (*x)[0][0] = INT64_MAX;
    (*y)[0][0] = -1;

    result_code = Matrix_Subtract(n, x, y, z);
    CU_ASSERT_EQUAL(kArithmeticOverflow, result_code);
  });
}

static void Matrix_Multiply_null_parameters() {
  const size_t n = 2;

  matrix_value(*x)[n][n] = NULL;
  matrix_value(*y)[n][n] = NULL;

  ResultCode result_code = Matrix_Initalize(n, &x);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_Initalize(n, &y);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_Multiply(n, x, y, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Matrix_Multiply(n, x, NULL, y);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Matrix_Multiply(n, NULL, x, y);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  free(x);
  free(y);
}

static void Matrix_Multiply_invalid_size() {
  SUT(2, {
    result_code = Matrix_Multiply(0, x, y, z);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
  });
}

static void Matrix_Multiply_happy_path() {
  SUT(2, {
    matrix_value(*expected)[n][n] = NULL;

    ResultCode result_code = Matrix_Initalize(n, &expected);
    CU_ASSERT_EQUAL(kSuccess, result_code);

    (*expected)[0][0] = 38;
    (*expected)[0][1] = 17;
    (*expected)[1][0] = 26;
    (*expected)[1][1] = 14;

    (*x)[0][0] = 1;
    (*x)[0][1] = 7;
    (*x)[1][0] = 2;
    (*x)[1][1] = 4;

    (*y)[0][0] = 3;
    (*y)[0][1] = 3;
    (*y)[1][0] = 5;
    (*y)[1][1] = 2;

    result_code = Matrix_Multiply(2, x, y, z);
    CU_ASSERT_EQUAL(kSuccess, result_code);
    CU_ASSERT_EQUAL(0, memcmp(z, expected, sizeof(*expected)));

    free(expected);
  });
}

static void Matrix_Multiply_arithmetic_overflow() {
  SUT(2, {
    (*x)[0][0] = INT64_MAX;
    (*y)[0][0] = 2;

    result_code = Matrix_Multiply(n, x, y, z);
    CU_ASSERT_EQUAL(kArithmeticOverflow, result_code);
  });
}

static void Matrix_Tiling_null_parameters() {
  const size_t n = 2;

  matrix_value(*x)[n][n] = NULL;
  matrix_value(*y)[n][n] = NULL;

  ResultCode result_code = Matrix_Initalize(n, &x);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_Initalize(n, &y);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_TilingMultiply(n, x, y, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Matrix_TilingMultiply(n, x, NULL, y);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Matrix_TilingMultiply(n, NULL, x, y);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  free(x);
  free(y);
}

static void Matrix_Tiling_invalid_size() {
  SUT(2, {
    result_code = Matrix_TilingMultiply(0, x, y, z);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
  });
}

static void Matrix_Tiling_happy_path() {
  SUT(2, {
    matrix_value(*expected)[n][n] = NULL;

    ResultCode result_code = Matrix_Initalize(n, &expected);
    CU_ASSERT_EQUAL(kSuccess, result_code);

    (*expected)[0][0] = 38;
    (*expected)[0][1] = 17;
    (*expected)[1][0] = 26;
    (*expected)[1][1] = 14;

    (*x)[0][0] = 1;
    (*x)[0][1] = 7;
    (*x)[1][0] = 2;
    (*x)[1][1] = 4;

    (*y)[0][0] = 3;
    (*y)[0][1] = 3;
    (*y)[1][0] = 5;
    (*y)[1][1] = 2;

    result_code = Matrix_TilingMultiply(2, x, y, z);
    CU_ASSERT_EQUAL(kSuccess, result_code);
    CU_ASSERT_EQUAL(0, memcmp(z, expected, sizeof(*expected)));

    free(expected);
  });
}

static void Matrix_Tiling_arithmetic_overflow() {
  SUT(2, {
    (*x)[0][0] = INT64_MAX;
    (*y)[0][0] = 2;

    result_code = Matrix_TilingMultiply(n, x, y, z);
    CU_ASSERT_EQUAL(kArithmeticOverflow, result_code);
  });
}

static void Matrix_StrassenMultiply_null_parameters() {
  const size_t n = 2;

  matrix_value(*x)[n][n] = NULL;
  matrix_value(*y)[n][n] = NULL;

  ResultCode result_code = Matrix_Initalize(n, &x);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_Initalize(n, &y);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_StrassenMultiply(n, x, y, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Matrix_StrassenMultiply(n, x, NULL, y);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Matrix_StrassenMultiply(n, NULL, x, y);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  free(x);
  free(y);
}

static void Matrix_StrassenMultiply_invalid_size() {
  SUT(2, {
    result_code = Matrix_StrassenMultiply(0, x, y, z);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);

    // Not a power of 2
    result_code = Matrix_StrassenMultiply(10, x, y, z);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
  });
}

static void Matrix_StrassenMultiply_happy_path() {
  SUT(2, {
    matrix_value(*expected)[n][n] = NULL;

    ResultCode result_code = Matrix_Initalize(n, &expected);
    CU_ASSERT_EQUAL(kSuccess, result_code);

    (*expected)[0][0] = 38;
    (*expected)[0][1] = 17;
    (*expected)[1][0] = 26;
    (*expected)[1][1] = 14;

    (*x)[0][0] = 1;
    (*x)[0][1] = 7;
    (*x)[1][0] = 2;
    (*x)[1][1] = 4;

    (*y)[0][0] = 3;
    (*y)[0][1] = 3;
    (*y)[1][0] = 5;
    (*y)[1][1] = 2;

    result_code = Matrix_StrassenMultiply(2, x, y, z);
    CU_ASSERT_EQUAL(kSuccess, result_code);
    CU_ASSERT_EQUAL(0, memcmp(z, expected, sizeof(*expected)));

    free(expected);
  });
}

static void Matrix_StrassenMultiply_arithmetic_overflow() {
  SUT(2, {
    (*x)[0][0] = INT64_MAX;
    (*y)[0][0] = 2;

    result_code = Matrix_StrassenMultiply(n, x, y, z);
    CU_ASSERT_EQUAL(kArithmeticOverflow, result_code);
  });
}

static void Matrix_StrassenMultiply_failed_malloc() {
#if !defined(NDEBUG)
  SUT(32, {
    FAILED_MALLOC_TEST({
      result_code = Matrix_StrassenMultiply(n, x, y, z);
      CU_ASSERT_EQUAL(kFailedMemoryAllocation, result_code);
    });
  })
#endif
}

static void Matrix_StrassenMultiply_matches_brute_force() {
  const size_t n = 64;
  typedef matrix_value(*matrix)[n][n];
  matrix x = NULL;
  matrix y = NULL;
  matrix z1 = NULL;
  matrix z2 = NULL;
  matrix z3 = NULL;

  ResultCode result_code = Matrices_Initalize(n, 5, &x, &y, &z1, &z2, &z3);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      (*x)[i][j] = random() % 100;
      (*y)[i][j] = random() % 100;
    }
  }

  result_code = Matrix_Multiply(n, x, y, z1);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_StrassenMultiply(n, x, y, z2);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_TilingMultiply(n, x, y, z3);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  CU_ASSERT_EQUAL(0, memcmp(z1, z2, sizeof(*x)));
  CU_ASSERT_EQUAL(0, memcmp(z1, z3, sizeof(*x)));

  Matrices_Destroy(5, x, y, z1, z2, z3);
}

int RegisterMatrixOperationsTests() {
  CU_TestInfo matrix_tests[] = {
      CU_TEST_INFO(Matrix_Initalize_failed_malloc),
      CU_TEST_INFO(Matrix_Initalize_invalid_size),
      CU_TEST_INFO(Matrix_Initalize_null_parameter),
      CU_TEST_INFO(Matrix_Initalize_non_null_output_parameter),
      CU_TEST_INFO(Matrix_Initalize_size_causes_overflow),
      CU_TEST_INFO(Matrix_Initalize_initalizes_to_zero),
      CU_TEST_INFO(Matrix_Add_null_parameters),
      CU_TEST_INFO(Matrix_Add_invalid_size),
      CU_TEST_INFO(Matrix_Add_happy_path),
      CU_TEST_INFO(Matrix_Add_arithmetic_overflow),
      CU_TEST_INFO(Matrix_Subtract_null_parameters),
      CU_TEST_INFO(Matrix_Subtract_invalid_size),
      CU_TEST_INFO(Matrix_Subtract_happy_path),
      CU_TEST_INFO(Matrix_Subtract_arithmetic_overflow),
      CU_TEST_INFO(Matrix_Multiply_null_parameters),
      CU_TEST_INFO(Matrix_Multiply_invalid_size),
      CU_TEST_INFO(Matrix_Multiply_happy_path),
      CU_TEST_INFO(Matrix_Multiply_arithmetic_overflow),
      CU_TEST_INFO(Matrix_Tiling_null_parameters),
      CU_TEST_INFO(Matrix_Tiling_invalid_size),
      CU_TEST_INFO(Matrix_Tiling_happy_path),
      CU_TEST_INFO(Matrix_Tiling_arithmetic_overflow),
      CU_TEST_INFO(Matrix_StrassenMultiply_null_parameters),
      CU_TEST_INFO(Matrix_StrassenMultiply_invalid_size),
      CU_TEST_INFO(Matrix_StrassenMultiply_happy_path),
      CU_TEST_INFO(Matrix_StrassenMultiply_arithmetic_overflow),
      CU_TEST_INFO(Matrix_StrassenMultiply_failed_malloc),
      CU_TEST_INFO(Matrix_StrassenMultiply_matches_brute_force),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Strassen Matrix Multiplication",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = matrix_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
