#include "./selection_sort.h"

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

int selection_sort(const size_t n, const size_t size, void* arr,
                   const comparator comparator) {
  if (n == 0 || size == 0 || arr == NULL || comparator == NULL) return -1;

  for (size_t i = 0; i < n - 1; i++) {
    size_t lowest_index = i;

    for (size_t j = i + 1; j < n; j++) {
      void* jth = (char*)arr + (j * size);
      void* lowest = (char*)arr + (lowest_index * size);
      int result = comparator(jth, lowest);

      if (result < 0) lowest_index = j;
    }

    if (i != lowest_index) {
      void* ith = (char*)arr + (i * size);
      void* lowest = (char*)arr + (lowest_index * size);
      swap(size, ith, lowest);
    }
  }

  return 0;
}
