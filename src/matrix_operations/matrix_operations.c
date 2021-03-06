/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "matrix_operations.h"

#include <stdbool.h>
#include <stdlib.h>

const size_t kStopRecursionSize = 16;

static matrix_value Min(matrix_value x, matrix_value y) {
  return y ^ ((x ^ y) & -(x < y));
}

static bool IsPowerOfTwo(matrix_value x) { return (x & (x - 1)) == 0; }

static void SplitMatrixInQuads(size_t n, const matrix_value (*x)[n * 2][n * 2],
                               matrix_value (*A)[n][n], matrix_value (*B)[n][n],
                               matrix_value (*C)[n][n],
                               matrix_value (*D)[n][n]) {
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      (*A)[i][j] = (*x)[i][j];
      (*B)[i][j] = (*x)[i][n + j];
      (*C)[i][j] = (*x)[n + i][j];
      (*D)[i][j] = (*x)[n + i][n + j];
    }
  }
}

static void JoinQuads(size_t n, matrix_value (*A)[n][n],
                      matrix_value (*B)[n][n], matrix_value (*C)[n][n],
                      matrix_value (*D)[n][n],
                      matrix_value (*result)[n * 2][n * 2]) {
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      (*result)[i][j] = (*A)[i][j];
      (*result)[i][j + n] = (*B)[i][j];
      (*result)[n + i][j] = (*C)[i][j];
      (*result)[n + i][n + j] = (*D)[i][j];
    }
  }
}

static ResultCode Matrix_RecursiveMultiplyRecursive(
    size_t n, const matrix_value (*x)[n][n], const matrix_value (*y)[n][n],
    matrix_value (*result)[n][n]) {
  if (n <= kStopRecursionSize) return Matrix_Multiply(n, x, y, result);

  ResultCode result_code;
  size_t sub_size = n >> 1;
  typedef matrix_value(*matrix)[sub_size][sub_size];

  matrix A11 = NULL;
  matrix A12 = NULL;
  matrix A21 = NULL;
  matrix A22 = NULL;
  matrix B11 = NULL;
  matrix B12 = NULL;
  matrix B21 = NULL;
  matrix B22 = NULL;
  matrix Q1 = NULL;
  matrix Q2 = NULL;
  matrix Q3 = NULL;
  matrix Q4 = NULL;
  matrix temp = NULL;
  matrix temp2 = NULL;

  result_code =
      Matrices_Initalize(sub_size, 14, &A11, &A12, &A21, &A22, &B11, &B12, &B21,
                         &B22, &Q1, &Q2, &Q3, &Q4, &temp, &temp2);
  if (result_code != kSuccess) goto done;

  SplitMatrixInQuads(sub_size, x, A11, A12, A21, A22);
  SplitMatrixInQuads(sub_size, y, B11, B12, B21, B22);

  // Quad 1
  result_code = Matrix_RecursiveMultiplyRecursive(sub_size, A11, B11, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_RecursiveMultiplyRecursive(sub_size, A12, B21, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, temp, temp2, Q1);
  if (result_code != kSuccess) goto done;

  // Quad 2
  result_code = Matrix_RecursiveMultiplyRecursive(sub_size, A11, B12, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_RecursiveMultiplyRecursive(sub_size, A12, B22, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, temp, temp2, Q2);
  if (result_code != kSuccess) goto done;

  // Quad 3
  result_code = Matrix_RecursiveMultiplyRecursive(sub_size, A21, B11, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_RecursiveMultiplyRecursive(sub_size, A22, B21, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, temp, temp2, Q3);
  if (result_code != kSuccess) goto done;

  // Quad 4
  result_code = Matrix_RecursiveMultiplyRecursive(sub_size, A21, B12, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_RecursiveMultiplyRecursive(sub_size, A22, B22, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, temp, temp2, Q4);
  if (result_code != kSuccess) goto done;

  JoinQuads(sub_size, Q1, Q2, Q3, Q4, result);

  result_code = kSuccess;

done:
  Matrices_Destroy(14, A11, A12, A21, A22, B11, B12, B21, B22, Q1, Q2, Q3, Q4,
                   temp, temp2);

  return result_code;
}

static ResultCode Matrix_StrassenMultiplyRecursive(
    size_t n, const matrix_value (*x)[n][n], const matrix_value (*y)[n][n],
    matrix_value (*result)[n][n]) {
  ResultCode result_code;

  if (n <= kStopRecursionSize) return Matrix_Multiply(n, x, y, result);

  size_t sub_size = n >> 1;
  typedef matrix_value(*matrix)[sub_size][sub_size];

  matrix A = NULL;
  matrix B = NULL;
  matrix C = NULL;
  matrix D = NULL;
  matrix E = NULL;
  matrix F = NULL;
  matrix G = NULL;
  matrix H = NULL;
  matrix P1 = NULL;
  matrix P2 = NULL;
  matrix P3 = NULL;
  matrix P4 = NULL;
  matrix P5 = NULL;
  matrix P6 = NULL;
  matrix P7 = NULL;
  matrix temp = NULL;
  matrix temp2 = NULL;

  result_code =
      Matrices_Initalize(sub_size, 17, &A, &B, &C, &D, &E, &F, &G, &H, &P1, &P2,
                         &P3, &P4, &P5, &P6, &P7, &temp, &temp2);
  if (result_code != kSuccess) goto done;

  SplitMatrixInQuads(sub_size, x, A, B, C, D);
  SplitMatrixInQuads(sub_size, y, E, F, G, H);

  // P1
  result_code = Matrix_Subtract(sub_size, F, H, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, A, temp, P1);
  if (result_code != kSuccess) goto done;

  // P2
  result_code = Matrix_Add(sub_size, A, B, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, temp, H, P2);
  if (result_code != kSuccess) goto done;

  // P3
  result_code = Matrix_Add(sub_size, C, D, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, temp, E, P3);
  if (result_code != kSuccess) goto done;

  // P4
  result_code = Matrix_Subtract(sub_size, G, E, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, D, temp, P4);
  if (result_code != kSuccess) goto done;

  // P5
  result_code = Matrix_Add(sub_size, A, D, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, E, H, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, temp, temp2, P5);
  if (result_code != kSuccess) goto done;

  // P6
  result_code = Matrix_Subtract(sub_size, B, D, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, G, H, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, temp, temp2, P6);
  if (result_code != kSuccess) goto done;

  // P7
  result_code = Matrix_Subtract(sub_size, A, C, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, E, F, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, temp, temp2, P7);
  if (result_code != kSuccess) goto done;

  // Quad 1
  result_code = Matrix_Add(sub_size, P4, P5, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Subtract(sub_size, temp, P2, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, temp2, P6, A);
  if (result_code != kSuccess) goto done;

  // Quad 2
  result_code = Matrix_Add(sub_size, P1, P2, B);
  if (result_code != kSuccess) goto done;

  // Quad 3
  result_code = Matrix_Add(sub_size, P3, P4, C);
  if (result_code != kSuccess) goto done;

  // Quad 4
  result_code = Matrix_Add(sub_size, P1, P5, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Subtract(sub_size, temp, P3, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Subtract(sub_size, temp2, P7, D);
  if (result_code != kSuccess) goto done;

  JoinQuads(sub_size, A, B, C, D, result);

  result_code = kSuccess;

done:
  Matrices_Destroy(17, A, B, C, D, E, F, G, H, P1, P2, P3, P4, P5, P6, P7, temp,
                   temp2);

  return result_code;
}

ResultCode Matrix_Initalize(size_t n, matrix_value (**result)[n][n]) {
  if (n == 0) return kArgumentOutOfRange;
  if (result == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;

  size_t size = 0;
  if (__builtin_mul_overflow(n, n, &size)) return kArithmeticOverflow;

  matrix_value* matrix = calloc(sizeof(matrix_value), size);
  if (matrix == NULL) return kFailedMemoryAllocation;

  *result = (matrix_value(*)[n][n])matrix;
  return kSuccess;
}

void Matrices_Destroy(const size_t count, ...) {
  va_list valist;
  va_start(valist, count);

  for (size_t i = 0; i < count; i++) {
    // Bug in clang-tidy that produces a false positive for
    // va_arg() is called on an uninitialized va_list
    // NOLINTNEXTLINE
    void* matrix = va_arg(valist, void*);
    free(matrix);
  }

  va_end(valist);
}

ResultCode Matrices_Initalize(const size_t n, const size_t count, ...) {
  ResultCode result_code = kSuccess;
  va_list valist;
  va_start(valist, count);

  for (size_t i = 0; i < count; i++) {
    // Bug in clang-tidy that produces a false positive for
    // va_arg() is called on an uninitialized va_list
    // NOLINTNEXTLINE
    matrix_value(**matrix)[n][n] = va_arg(valist, void*);
    result_code = Matrix_Initalize(n, matrix);
    if (result_code != kSuccess) break;
  }

  va_end(valist);
  return result_code;
}

ResultCode Matrix_Transpose(size_t n, const matrix_value (*x)[n][n],
                            matrix_value (*result)[n][n]) {
  if (x == NULL || result == NULL) return kNullParameter;
  if (n == 0) return kArgumentOutOfRange;

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      (*result)[j][i] = (*x)[i][j];
    }
  }

  return kSuccess;
}

ResultCode Matrix_Add(size_t n, const matrix_value (*x)[n][n],
                      const matrix_value (*y)[n][n],
                      matrix_value (*result)[n][n]) {
  if (x == NULL || y == NULL || result == NULL) return kNullParameter;
  if (n == 0) return kArgumentOutOfRange;

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      if (__builtin_add_overflow((*x)[i][j], (*y)[i][j], &(*result)[i][j])) {
        return kArithmeticOverflow;
      }
    }
  }

  return kSuccess;
}

ResultCode Matrix_Subtract(size_t n, const matrix_value (*x)[n][n],
                           const matrix_value (*y)[n][n],
                           matrix_value (*result)[n][n]) {
  if (x == NULL || y == NULL || result == NULL) return kNullParameter;
  if (n == 0) return kArgumentOutOfRange;

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      if (__builtin_sub_overflow((*x)[i][j], (*y)[i][j], &(*result)[i][j])) {
        return kArithmeticOverflow;
      }
    }
  }

  return kSuccess;
}

ResultCode Matrix_Multiply(size_t n, const matrix_value (*x)[n][n],
                           const matrix_value (*y)[n][n],
                           matrix_value (*result)[n][n]) {
  if (x == NULL || y == NULL || result == NULL) return kNullParameter;
  if (n == 0) return kArgumentOutOfRange;

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      (*result)[i][j] = 0;
      for (size_t k = 0; k < n; k++) {
        matrix_value temp;
        if (__builtin_mul_overflow((*x)[i][k], (*y)[k][j], &temp)) {
          return kArithmeticOverflow;
        }

        if (__builtin_add_overflow((*result)[i][j], temp, &(*result)[i][j])) {
          return kArithmeticOverflow;
        }
      }
    }
  }

  return kSuccess;
}

ResultCode Matrix_TilingMultiply(size_t n, const matrix_value (*x)[n][n],
                                 const matrix_value (*y)[n][n],
                                 matrix_value (*result)[n][n]) {
  if (x == NULL || y == NULL || result == NULL) return kNullParameter;
  if (n == 0) return kArgumentOutOfRange;
  const size_t block_size = 64 / sizeof(matrix_value);

  for (size_t ii = 0; ii < n; ii += block_size) {
    for (size_t jj = 0; jj < n; jj += block_size) {
      for (size_t kk = 0; kk < n; kk += block_size) {
        size_t ilimit = Min(ii + block_size, n);
        for (size_t i = ii; i < ilimit; i++) {
          size_t jlimit = Min(jj + block_size, n);
          for (size_t j = jj; j < jlimit; j++) {
            size_t klimit = Min(kk + block_size, n);
            for (size_t k = kk; k < klimit; k++) {
              matrix_value temp;
              if (__builtin_mul_overflow((*x)[i][k], (*y)[k][j], &temp)) {
                return kArithmeticOverflow;
              }

              if (__builtin_add_overflow((*result)[i][j], temp,
                                         &(*result)[i][j])) {
                return kArithmeticOverflow;
              }
            }
          }
        }
      }
    }
  }

  return kSuccess;
}

ResultCode Matrix_TransposeMultiply(size_t n, const matrix_value (*x)[n][n],
                                    const matrix_value (*y)[n][n],
                                    matrix_value (*result)[n][n]) {
  if (x == NULL || y == NULL || result == NULL) return kNullParameter;
  if (n == 0) return kArgumentOutOfRange;
  ResultCode result_code;

  matrix_value(*transposed)[n][n] = NULL;
  result_code = Matrix_Initalize(n, &transposed);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Transpose(n, y, transposed);
  if (result_code != kSuccess) goto done;

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      for (size_t k = 0; k < n; k++) {
        matrix_value temp;
        if (__builtin_mul_overflow((*x)[i][k], (*transposed)[j][k], &temp)) {
          result_code = kArithmeticOverflow;
          goto done;
        }

        if (__builtin_add_overflow((*result)[i][j], temp, &(*result)[i][j])) {
          result_code = kArithmeticOverflow;
          goto done;
        }
      }
    }
  }

done:

  free(transposed);
  return result_code;
}

ResultCode Matrix_RecursiveMultiply(size_t n, const matrix_value (*x)[n][n],
                                    const matrix_value (*y)[n][n],
                                    matrix_value (*result)[n][n]) {
  if (x == NULL || y == NULL || result == NULL) return kNullParameter;
  if (n == 0) return kArgumentOutOfRange;

  return Matrix_RecursiveMultiplyRecursive(n, x, y, result);
}

ResultCode Matrix_StrassenMultiply(size_t n, const matrix_value (*x)[n][n],
                                   const matrix_value (*y)[n][n],
                                   matrix_value (*result)[n][n]) {
  if (x == NULL || y == NULL || result == NULL) return kNullParameter;
  if (n == 0 || !IsPowerOfTwo(n)) return kArgumentOutOfRange;

  return Matrix_StrassenMultiplyRecursive(n, x, y, result);
}
