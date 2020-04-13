#include <limits.h>
#include <math.h>
#include <stdarg.h>

#include "Math.h"

int min(size_t count, ...) {
  int min_val = INT_MAX;
  va_list valist;
  va_start(valist, count);

  for (size_t i = 0; i < count; i++) {
    int val = va_arg(valist, int);  // NOLINT
    min_val = (val < min_val) ? val : min_val;
  }

  va_end(valist);
  return min_val;
}

double min_double(size_t count, ...) {
  double min_val = INFINITY;
  va_list valist;
  va_start(valist, count);

  for (size_t i = 0; i < count; i++) {
    double val = va_arg(valist, double);  // NOLINT
    min_val = (val < min_val) ? val : min_val;
  }

  va_end(valist);
  return min_val;
}
