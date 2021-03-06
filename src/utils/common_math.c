/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "common_math.h"

#include <limits.h>
#include <math.h>
#include <stdarg.h>

int Min(const size_t count, ...) {
  int min_val = INT_MAX;
  va_list valist;
  va_start(valist, count);

  for (size_t i = 0; i < count; i++) {
    int val = va_arg(valist, int);
    min_val = (val < min_val) ? val : min_val;
  }

  va_end(valist);
  return min_val;
}

double MinDouble(const size_t count, ...) {
  double min_val = INFINITY;
  va_list valist;
  va_start(valist, count);

  for (size_t i = 0; i < count; i++) {
    double val = va_arg(valist, double);
    min_val = (val < min_val) ? val : min_val;
  }

  va_end(valist);
  return min_val;
}

void* MaxArray(const size_t n, const size_t size, void* arr,
               const sort_strategy comparator) {
  if (arr == NULL || comparator == NULL || size == 0 || n == 0) return NULL;

  void* max = arr;

  for (size_t i = 1; i < n; i++) {
    void* ith = (char*)arr + i * size;
    if (comparator(max, ith) < 0) max = ith;
  }

  return max;
}

void* MinArray(const size_t n, const size_t size, void* arr,
               const sort_strategy comparator) {
  if (arr == NULL || comparator == NULL || size == 0 || n == 0) return NULL;

  void* min = arr;

  for (size_t i = 1; i < n; i++) {
    void* ith = (char*)arr + i * size;
    if (comparator(min, ith) > 0) min = ith;
  }

  return min;
}

size_t NextPowerOfTwo(size_t val) {
  size_t count = 0;

  if (val && !(val & (val - 1))) return val;

  while (val != 0) {
    val >>= 1;
    count += 1;
  }

  return 1 << count;
}
