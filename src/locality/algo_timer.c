// Copyright 2020 Dale Alleshouse
#include "algo_timer.h"

#include <time.h>

#include "locality.h"

double EnumerateMultiDimArray(const size_t n, const IterationType type) {
  int64_t md_array[n][n];

  clock_t t = clock();

  ResultCode result;

  switch (type) {
    case kRowOrder:
      result = RowOrder(n, md_array);
      break;
    case kColumnOrder:
      result = ColumnOrder(n, md_array);
      break;
    default:
      return -1;
  }

  if (result != kSuccess) return -1;

  t = clock() - t;

  double time = ((double)t) / CLOCKS_PER_SEC;
  return time;
}
