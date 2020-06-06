#include "./bubble_sort.h"

#include <stdbool.h>
#include <string.h>

static int swap(const size_t size, void* x, void* y) {
  if (size == 0 || x == NULL || y == NULL) return -1;

  char n[size];
  memcpy(n, x, size);
  memcpy(x, y, size);
  memcpy(y, n, size);

  return 0;
}

int bubble_sort(const size_t n, const size_t size, void* arr,
                const comparator comparator) {
  if (n == 0 || size == 0 || arr == NULL || comparator == NULL) return -1;

  size_t unsorted_to = n - 1;
  bool sorted = false;

  while (!sorted) {
    sorted = true;

    for (size_t i = 0; i < unsorted_to; i++) {
      void* ith = (char*)arr + (i * size);
      void* ith_1 = (char*)arr + (i * size) + size;

      int result = comparator(ith, ith_1);

      if (result > 0) {
        swap(size, ith, ith_1);
        sorted = false;
      }
    }

    unsorted_to--;
  }

  return 0;
}
