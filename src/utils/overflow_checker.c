#include "./overflow_checker.h"
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

bool is_mul_overflow_size_t(size_t x, size_t y) {
  if (x == 0 || y == 0) return false;

  size_t result = x * y;

  if (x == result / y) return false;

  return true;
}

bool is_add_overflow_size_t(size_t x, size_t y) {
  if (x > SIZE_MAX - y) return true;

  return false;
}

bool is_add_overflow_int(int x, int y) {
  if (x > 0 && y > INT_MAX - x) return true;

  if (x < 0 && y < INT_MIN - x) return true;

  return false;
}

bool is_add_overflow_uint(unsigned int x, unsigned int y) {
  if (x > 0 && y > UINT_MAX - x) return true;

  return false;
}

bool is_add_overflow_ulong(unsigned long x, unsigned long y) {
  if (x > 0 && y > ULONG_MAX - x) return true;

  return false;
}

bool is_mul_overflow_ulong(unsigned long x, unsigned long y) {
  return (x > ULONG_MAX / y);
}
