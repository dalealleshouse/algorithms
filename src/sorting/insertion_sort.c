#include "./insertion_sort.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static void* duplicate(const size_t size, void* ptr) {
  void* tmp = malloc(size);
  memcpy(tmp, ptr, size);
  return tmp;
}

static void shift_array(const size_t size, void* ptr) {
  memmove((char*)ptr + size, ptr, size);
}

int insertion_sort(const size_t n, const size_t size, void* arr,
                   const comparator comparator) {
  if (n == 0 || size == 0 || arr == NULL || comparator == NULL) return -1;

  for (size_t i = 1; i < n; i++) {
    int open_index = i;
    void* ith = duplicate(size, (char*)arr + (i * size));

    for (int j = i - 1; j >= 0; j--) {
      void* jth = (char*)arr + (j * size);

      int result = comparator(jth, ith);
      if (result > 0) {
        shift_array(size, jth);
        open_index--;
      } else
        break;
    }

    memcpy((char*)arr + (open_index * size), ith, size);
    free(ith);
  }

  return 0;
}
