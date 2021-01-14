/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "algo_timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common_math.h"
#include "matrix_operations.h"

typedef ResultCode (*matrix_algo)(size_t n, matrix_value (*x)[n][n],
                                  matrix_value (*y)[n][n],
                                  matrix_value (*result)[n][n]);

static matrix_algo GetAlgorithm(const MatrixAlgoirthm algo) {
  switch (algo) {
    case kNaiveMatrixMultiply:
      return Matrix_Multiply;
    case kTilingMatrixMultiply:
      return Matrix_TilingMultiply;
    case kStrassenMatrixMultiply:
      return Matrix_StrassenMultiply;
    default:
      return NULL;
  }
}

double MatrixMultiplyTime(const size_t n, const MatrixAlgoirthm algo_type) {
  typedef matrix_value(*matrix)[n][n];

  double time = -1;
  matrix x = NULL;
  matrix y = NULL;
  matrix z = NULL;

  // get a function pointer to the desired algorithm
  matrix_algo algo = GetAlgorithm(algo_type);
  if (algo == NULL) {
    fprintf(stderr, "Invalid matrix algorithm: %d\n", algo_type);
    return -1;
  }

  // malloc all the matracies
  ResultCode result_code = Matrices_Initalize(n, 3, &x, &y, &z);
  if (result_code != kSuccess) {
    fprintf(stderr, "%s\n", Result_ErrorMessage(result_code));
    return time;
  }

  // Populate the matracies with random data
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      (*x)[i][j] = random() % 100;
      (*y)[i][j] = random() % 100;
    }
  }

  // start the timer
  clock_t t = clock();

  // run the algorithm
  result_code = algo(n, x, y, z);

  // stop the timer
  t = clock() - t;

  if (result_code != kSuccess) {
    fprintf(stderr, "Matrix Algorithm returned an error code: %s",
            Result_ErrorMessage(result_code));
    goto done;
  }

  time = ((double)t) / CLOCKS_PER_SEC;

done:
  Matrices_Destroy(3, x, y, z);
  return time;
}
