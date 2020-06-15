// Copyright 2020 Dale Alleshouse
#include "locality.h"

#include <stdlib.h>

ResultCode ColumnOrder(size_t n, int64_t array[n][n]) {
  if (array == NULL) return kNullParameter;

  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      array[j][i] ^= array[j][i];
    }
  }

  return kSuccess;
}

ResultCode RowOrder(size_t n, int64_t array[n][n]) {
  if (array == NULL) return kNullParameter;

  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      array[i][j] ^= array[i][j];
    }
  }

  return kSuccess;
}
