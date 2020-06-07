#include "./strassen.h"

#include <time.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* ispower2 ****************/

void ispowerof2_return_false_for_invalid() {
  CU_ASSERT_FALSE(ispowerof2(3));
  CU_ASSERT_FALSE(ispowerof2(7));
  CU_ASSERT_FALSE(ispowerof2(6));
}

void ispowerof2_return_true_for_valid() {
  CU_ASSERT_TRUE(ispowerof2(2));
  CU_ASSERT_TRUE(ispowerof2(4));
  CU_ASSERT_TRUE(ispowerof2(8));
}

int RegisterIsPowerOfTwoTests() {
  CU_pSuite pSuite = NULL;
  pSuite = CU_add_suite("ispowerof2", init_suite, clean_suite);
  if (NULL == pSuite) {
    return -1;
  }

  /* add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "non power of 2 returns false",
                           ispowerof2_return_false_for_invalid) ||
       NULL == CU_add_test(pSuite, "power of 2 return true",
                           ispowerof2_return_true_for_valid))

  ) {
    return CU_get_error();
  }

  return 0;
}

/************* multiply matracies **************/
void compare_matrices(const int rows, const int columns,
                      const int expected[rows][columns],
                      const int sut[rows][columns]) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      CU_ASSERT_EQUAL(expected[i][j], sut[i][j]);
    }
  }
}

void MultiplySquarMatrices_NullDoesNotThrow(void) {
  int result = multiply_square_matrices(1, NULL, NULL, NULL);
  CU_ASSERT_EQUAL(result, -1);
}

void multiply_size_one_matrix(void) {
  int expected[1][1] = {{25}};
  int x[][1] = {{5}};
  int y[][1] = {{5}};
  int output[1][1];

  int result = multiply_square_matrices(1, x, y, output);

  CU_ASSERT_EQUAL(result, 0);
  compare_matrices(1, 1, expected, output);
}

void multiply_square_matrix(void) {
  int expected[2][2] = {{19, 22}, {43, 50}};
  int x[][2] = {{1, 2}, {3, 4}};
  int y[][2] = {{5, 6}, {7, 8}};
  int output[2][2];

  int result = multiply_square_matrices(2, x, y, output);

  CU_ASSERT_EQUAL(result, 0);
  compare_matrices(2, 2, expected, output);
}

void multiply_square_matrix_n_equal_8(void) {
  int expected[2][2] = {{19, 22}, {43, 50}};
  int x[][2] = {{1, 2}, {3, 4}};
  int y[][2] = {{5, 6}, {7, 8}};
  int output[2][2];

  int result = multiply_square_matrices(2, x, y, output);

  CU_ASSERT_EQUAL(result, 0);
  compare_matrices(2, 2, expected, output);
}

void return_error_for_non_power_of_2(void) {
  int n = 8;
  const int expected[8][8] = {{36, 36, 36, 36, 36, 36, 36, 36},
                              {72, 72, 72, 72, 72, 72, 72, 72},
                              {108, 108, 108, 108, 108, 108, 108, 108},
                              {144, 144, 144, 144, 144, 144, 144, 144},
                              {180, 180, 180, 180, 180, 180, 180, 180},
                              {216, 216, 216, 216, 216, 216, 216, 216},
                              {252, 252, 252, 252, 252, 252, 252, 252},
                              {288, 288, 288, 288, 288, 288, 288, 288}};

  int x[n][n];
  int output[n][n];

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      x[i][j] = i + 1;
    }
  }

  int result = multiply_square_matrices(n, x, x, output);

  CU_ASSERT_EQUAL(result, 0);
  compare_matrices(2, 2, expected, output);
}

void matches_brute_force(void) {
  const int n = 32;

  int bf[n][n];
  int st[n][n];
  int a[n][n];
  int b[n][n];

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      a[i][j] = rand() % 100 + 1;
      b[i][j] = rand() % 100 + 1;
    }
  }

  int result = multiply_square_matrices(n, a, b, st);
  CU_ASSERT_EQUAL_FATAL(result, 0);

  result = brute_force(n, a, b, bf);
  CU_ASSERT_EQUAL_FATAL(result, 0);

  compare_matrices(n, n, bf, st);
}

int RegisterMultiplySquareMatricesTests() {
  CU_pSuite pSuite = NULL;
  pSuite = CU_add_suite("compare_matrices", init_suite, clean_suite);
  if (NULL == pSuite) {
    return -1;
  }

  /* add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "null input does not cause error",
                           MultiplySquarMatrices_NullDoesNotThrow)) ||
      (NULL ==
       CU_add_test(pSuite, "multply size one matrix", multiply_size_one_matrix))

      || (NULL == CU_add_test(pSuite, "multply square matrix",
                              multiply_square_matrix)) ||
      (NULL == CU_add_test(pSuite, "multply square matrix of size 8",
                           multiply_square_matrix_n_equal_8)) ||
      (NULL == CU_add_test(pSuite, "strassen matches brute force",
                           matches_brute_force)) ||
      (NULL == CU_add_test(pSuite, "return error for non power of 2",
                           return_error_for_non_power_of_2))) {
    return CU_get_error();
  }

  return 0;
}

extern int RegisterIsPowerOfTwoTests();
extern int RegisterMultiplySquareMatricesTests();
