/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include <stdlib.h>

#include "CUnit/CUnit.h"
#include "matrix_operations.h"
#include "test_helpers.h"

#if !defined(NDEBUG)
#include "malloc_test_wrapper.h"
#endif

#define SUT(N, code_block)                                         \
  {                                                                \
    const size_t n = N;                                            \
                                                                   \
    matrix_value(*x)[n][n] = NULL;                                 \
    matrix_value(*y)[n][n] = NULL;                                 \
    matrix_value(*z)[n][n] = NULL;                                 \
                                                                   \
    ResultCode result_code = Matrices_Initalize(n, 3, &x, &y, &z); \
    CU_ASSERT_EQUAL(kSuccess, result_code);                        \
                                                                   \
    {code_block}                                                   \
                                                                   \
    Matrices_Destroy(3, x, y, z);                                  \
  }

#define TEST_MULTIPLY_ALGO(algorithm)                             \
  static void algorithm##_null_parameters() {                     \
    const size_t n = 2;                                           \
                                                                  \
    matrix_value(*x)[n][n] = NULL;                                \
    matrix_value(*y)[n][n] = NULL;                                \
                                                                  \
    ResultCode result_code = Matrices_Initalize(n, 2, &x, &y);    \
    CU_ASSERT_EQUAL(kSuccess, result_code);                       \
                                                                  \
    result_code = algorithm(n, x, y, NULL);                       \
    CU_ASSERT_EQUAL(kNullParameter, result_code);                 \
                                                                  \
    result_code = algorithm(n, x, NULL, y);                       \
    CU_ASSERT_EQUAL(kNullParameter, result_code);                 \
                                                                  \
    result_code = algorithm(n, NULL, x, y);                       \
    CU_ASSERT_EQUAL(kNullParameter, result_code);                 \
                                                                  \
    Matrices_Destroy(2, x, y);                                    \
  }                                                               \
                                                                  \
  static void algorithm##_invalid_size() {                        \
    SUT(2, {                                                      \
      result_code = algorithm(0, x, y, z);                        \
      CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);          \
    });                                                           \
  }                                                               \
                                                                  \
  static void algorithm##_happy_path() {                          \
    SUT(2, {                                                      \
      matrix_value(*expected)[n][n] = NULL;                       \
                                                                  \
      ResultCode result_code = Matrix_Initalize(n, &expected);    \
      CU_ASSERT_EQUAL(kSuccess, result_code);                     \
                                                                  \
      (*expected)[0][0] = 38;                                     \
      (*expected)[0][1] = 17;                                     \
      (*expected)[1][0] = 26;                                     \
      (*expected)[1][1] = 14;                                     \
                                                                  \
      (*x)[0][0] = 1;                                             \
      (*x)[0][1] = 7;                                             \
      (*x)[1][0] = 2;                                             \
      (*x)[1][1] = 4;                                             \
                                                                  \
      (*y)[0][0] = 3;                                             \
      (*y)[0][1] = 3;                                             \
      (*y)[1][0] = 5;                                             \
      (*y)[1][1] = 2;                                             \
                                                                  \
      result_code = algorithm(2, x, y, z);                        \
      CU_ASSERT_EQUAL(kSuccess, result_code);                     \
      CU_ASSERT_EQUAL(0, memcmp(z, expected, sizeof(*expected))); \
                                                                  \
      free(expected);                                             \
    });                                                           \
  }                                                               \
                                                                  \
  static void algorithm##_arithmetic_overflow() {                 \
    SUT(2, {                                                      \
      (*x)[0][0] = INT64_MAX;                                     \
      (*y)[0][0] = 2;                                             \
                                                                  \
      result_code = algorithm(n, x, y, z);                        \
      CU_ASSERT_EQUAL(kArithmeticOverflow, result_code);          \
    });                                                           \
  }

#define TEST_MULTIPLY_CASES(algorithm)        \
  CU_TEST_INFO(algorithm##_null_parameters),  \
      CU_TEST_INFO(algorithm##_invalid_size), \
      CU_TEST_INFO(algorithm##_happy_path),   \
      CU_TEST_INFO(algorithm##_arithmetic_overflow)

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

static void Matrix_Transpose_null_parameters() {
  const size_t n = 2;

  matrix_value(*x)[n][n] = NULL;

  ResultCode result_code = Matrix_Initalize(n, &x);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  result_code = Matrix_Transpose(n, x, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  result_code = Matrix_Transpose(n, NULL, x);
  CU_ASSERT_EQUAL(kNullParameter, result_code);

  free(x);
}

static void Matrix_Transpose_invalid_size() {
  SUT(2, {
    result_code = Matrix_Transpose(0, x, z);
    CU_ASSERT_EQUAL(kArgumentOutOfRange, result_code);
  });
}

static void Matrix_Transpose_happy_path() {
  SUT(3, {
    (*z)[0][0] = 5;
    (*z)[0][1] = 4;
    (*z)[0][2] = 7;
    (*z)[1][0] = 4;
    (*z)[1][1] = 0;
    (*z)[1][2] = 10;
    (*z)[2][0] = 3;
    (*z)[2][1] = 4;
    (*z)[2][2] = 3;

    (*x)[0][0] = 5;
    (*x)[0][1] = 4;
    (*x)[0][2] = 3;
    (*x)[1][0] = 4;
    (*x)[1][1] = 0;
    (*x)[1][2] = 4;
    (*x)[2][0] = 7;
    (*x)[2][1] = 10;
    (*x)[2][2] = 3;

    result_code = Matrix_Transpose(n, x, y);

    CU_ASSERT_EQUAL(kSuccess, result_code);
    CU_ASSERT_EQUAL(0, memcmp(y, z, sizeof(*z)));
  });
}

TEST_MULTIPLY_ALGO(Matrix_Multiply)
TEST_MULTIPLY_ALGO(Matrix_TilingMultiply)
TEST_MULTIPLY_ALGO(Matrix_TransposeMultiply)
TEST_MULTIPLY_ALGO(Matrix_StrassenMultiply)
TEST_MULTIPLY_ALGO(Matrix_RecursiveMultiply)

static void Matrix_TransposeMultiply_failed_malloc() {
#if !defined(NDEBUG)
  SUT(32, {
    FAILED_MALLOC_TEST({
      result_code = Matrix_TransposeMultiply(n, x, y, z);
      CU_ASSERT_EQUAL(kFailedMemoryAllocation, result_code);
    });
  })
#endif
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

static void Matrix_Multiply_all_match() {
  const size_t n = 64;
  typedef matrix_value(*matrix)[n][n];
  typedef ResultCode (*matrix_algo)(size_t n, const matrix_value(*x)[n][n],
                                    const matrix_value(*y)[n][n],
                                    matrix_value(*result)[n][n]);

  const size_t algo_count = 4;
  matrix_algo algos[4] = {Matrix_TilingMultiply, Matrix_TransposeMultiply,
                          Matrix_RecursiveMultiply, Matrix_StrassenMultiply};

  matrix x = NULL;
  matrix y = NULL;
  matrix expected = NULL;

  ResultCode result_code = Matrices_Initalize(n, 3, &x, &y, &expected);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      (*x)[i][j] = random() % 100;
      (*y)[i][j] = random() % 100;
    }
  }

  result_code = Matrix_Multiply(n, x, y, expected);
  CU_ASSERT_EQUAL(kSuccess, result_code);

  for (size_t i = 0; i < algo_count; i++) {
    matrix algo_result = NULL;
    result_code = Matrix_Initalize(n, &algo_result);
    CU_ASSERT_EQUAL(kSuccess, result_code);

    result_code = algos[i](n, x, y, algo_result);
    CU_ASSERT_EQUAL(kSuccess, result_code);
    CU_ASSERT_EQUAL(0, memcmp(expected, algo_result, sizeof(*x)));

    free(algo_result);
  }

  Matrices_Destroy(3, x, y, expected);
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
      CU_TEST_INFO(Matrix_Transpose_null_parameters),
      CU_TEST_INFO(Matrix_Transpose_invalid_size),
      CU_TEST_INFO(Matrix_Transpose_happy_path),
      CU_TEST_INFO(Matrix_Subtract_null_parameters),
      CU_TEST_INFO(Matrix_Subtract_invalid_size),
      CU_TEST_INFO(Matrix_Subtract_happy_path),
      CU_TEST_INFO(Matrix_Subtract_arithmetic_overflow),
      TEST_MULTIPLY_CASES(Matrix_Multiply),
      TEST_MULTIPLY_CASES(Matrix_TilingMultiply),
      TEST_MULTIPLY_CASES(Matrix_TransposeMultiply),
      TEST_MULTIPLY_CASES(Matrix_RecursiveMultiply),
      TEST_MULTIPLY_CASES(Matrix_StrassenMultiply),
      CU_TEST_INFO(Matrix_TransposeMultiply_failed_malloc),
      CU_TEST_INFO(Matrix_StrassenMultiply_failed_malloc),
      CU_TEST_INFO(Matrix_Multiply_all_match),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Strassen Matrix Multiplication",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = matrix_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
