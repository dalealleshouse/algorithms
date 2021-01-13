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

static bool IsPowerOfTwo(matrix_value x) { return (x & (x - 1)) == 0; }

static ResultCode SplitMatrixInQuads(size_t n, matrix_value (*x)[n][n],
                                     matrix_value (**A)[n][n],
                                     matrix_value (**B)[n][n],
                                     matrix_value (**C)[n][n],
                                     matrix_value (**D)[n][n]) {
  size_t sub_size = n >> 1;
  ResultCode result_code = Matrices_Initalize(sub_size, 4, A, B, C, D);
  if (result_code != kSuccess) return result_code;

  typedef matrix_value(*matrix)[sub_size][sub_size];
  matrix A1 = *A;
  matrix B1 = *B;
  matrix C1 = *C;
  matrix D1 = *D;

  for (size_t i = 0; i < sub_size; i++) {
    for (size_t j = 0; j < sub_size; j++) {
      (*A1)[i][j] = (*x)[i][j];
      (*B1)[i][j] = (*x)[i][sub_size + j];
      (*C1)[i][j] = (*x)[sub_size + i][j];
      (*D1)[i][j] = (*x)[sub_size + i][sub_size + j];
    }
  }

  return kSuccess;
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

static ResultCode Matrix_StrassenMultiplyRecursive(
    size_t n, matrix_value (*x)[n][n], matrix_value (*y)[n][n],
    matrix_value (*result)[n][n]) {
  ResultCode result_code;

  if (n == 1) return Matrix_Multiply(n, x, y, result);

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
  matrix S1 = NULL;
  matrix S2 = NULL;
  matrix S3 = NULL;
  matrix S4 = NULL;
  matrix S5 = NULL;
  matrix S6 = NULL;
  matrix S7 = NULL;
  matrix temp = NULL;
  matrix temp2 = NULL;

  result_code = SplitMatrixInQuads(n, x, &A, &B, &C, &D);
  if (result_code != kSuccess) goto done;

  result_code = SplitMatrixInQuads(n, y, &E, &F, &G, &H);
  if (result_code != kSuccess) goto done;

  result_code = Matrices_Initalize(sub_size, 9, &S1, &S2, &S3, &S4, &S5, &S6,
                                   &S7, &temp, &temp2);
  if (result_code != kSuccess) goto done;

  // S1
  result_code = Matrix_Subtract(sub_size, F, H, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, A, temp, S1);
  if (result_code != kSuccess) goto done;

  // S2
  result_code = Matrix_Add(sub_size, A, B, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, temp, H, S2);
  if (result_code != kSuccess) goto done;

  // S3
  result_code = Matrix_Add(sub_size, C, D, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, temp, E, S3);
  if (result_code != kSuccess) goto done;

  // S4
  result_code = Matrix_Subtract(sub_size, G, E, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, D, temp, S4);
  if (result_code != kSuccess) goto done;

  // S5
  result_code = Matrix_Add(sub_size, A, D, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, E, H, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, temp, temp2, S5);
  if (result_code != kSuccess) goto done;

  // S6
  result_code = Matrix_Subtract(sub_size, B, D, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, G, H, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, temp, temp2, S6);
  if (result_code != kSuccess) goto done;

  // S7
  result_code = Matrix_Subtract(sub_size, A, C, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, E, F, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_StrassenMultiplyRecursive(sub_size, temp, temp2, S7);
  if (result_code != kSuccess) goto done;

  // Quad 1
  result_code = Matrix_Add(sub_size, S4, S5, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Subtract(sub_size, temp, S2, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Add(sub_size, temp2, S6, A);
  if (result_code != kSuccess) goto done;

  // Quad 2
  result_code = Matrix_Add(sub_size, S1, S2, B);
  if (result_code != kSuccess) goto done;

  // Quad 3
  result_code = Matrix_Add(sub_size, S3, S4, C);
  if (result_code != kSuccess) goto done;

  // Quad 4
  result_code = Matrix_Add(sub_size, S1, S5, temp);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Subtract(sub_size, temp, S3, temp2);
  if (result_code != kSuccess) goto done;

  result_code = Matrix_Subtract(sub_size, temp2, S7, D);
  if (result_code != kSuccess) goto done;

  JoinQuads(sub_size, A, B, C, D, result);

  result_code = kSuccess;

done:
  Matrices_Destroy(17, A, B, C, D, E, F, G, H, S1, S2, S3, S4, S5, S6, S7, temp,
                   temp2);

  return result_code;
}

ResultCode Matrix_Initalize(size_t n, matrix_value (**result)[n][n]) {
  if (n == 0) return kArgumentOutOfRange;
  if (result == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;

  size_t size = 0;
  if (__builtin_mul_overflow(n, n, &size)) return kArithmeticOverflow;

  matrix_value* matrix = calloc(sizeof(matrix_value), n * n);
  if (matrix == NULL) return kFailedMemoryAllocation;

  *result = (matrix_value(*)[n][n])matrix;
  return kSuccess;
}

ResultCode Matrix_Add(size_t n, matrix_value (*x)[n][n],
                      matrix_value (*y)[n][n], matrix_value (*result)[n][n]) {
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

ResultCode Matrix_Subtract(size_t n, matrix_value (*x)[n][n],
                           matrix_value (*y)[n][n],
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

ResultCode Matrix_Multiply(size_t n, matrix_value (*x)[n][n],
                           matrix_value (*y)[n][n],
                           matrix_value (*result)[n][n]) {
  if (x == NULL || y == NULL || result == NULL) return kNullParameter;
  if (n == 0) return kArgumentOutOfRange;

  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
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

void Matrices_Destroy(const size_t count, ...) {
  va_list valist;
  va_start(valist, count);

  for (size_t i = 0; i < count; i++) {
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
    matrix_value(**matrix)[n][n] = va_arg(valist, void*);
    result_code = Matrix_Initalize(n, matrix);
    if (result_code != kSuccess) break;
  }

  va_end(valist);
  return result_code;
}

ResultCode Matrix_StrassenMultiply(size_t n, matrix_value (*x)[n][n],
                                   matrix_value (*y)[n][n],
                                   matrix_value (*result)[n][n]) {
  if (x == NULL || y == NULL || result == NULL) return kNullParameter;
  if (n == 0 || !IsPowerOfTwo(n)) return kArgumentOutOfRange;

  return Matrix_StrassenMultiplyRecursive(n, x, y, result);
}