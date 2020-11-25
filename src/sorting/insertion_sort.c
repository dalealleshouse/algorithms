/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "insertion_sort.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "sort_instrumentation.h"

static void* Duplicate(const size_t size, void* ptr) {
  void* tmp = malloc(size);
  memcpy(tmp, ptr, size);
  return tmp;
}

static void ShiftArray(const size_t size, void* ptr) {
  ++swap_count;
  memmove((char*)ptr + size, ptr, size);
}

ResultCode InsertionSort(const size_t n, const size_t size, void* arr,
                         const sort_strategy comparator) {
  if (arr == NULL || comparator == NULL) return kNullParameter;
  if (n == 0 || size == 0) return kArgumentOutOfRange;

  for (size_t i = 1; i < n; i++) {
    int open_index = i;
    void* ith = Duplicate(size, (char*)arr + (i * size));

    for (int j = (int)i - 1; j >= 0; j--) {
      void* jth = (char*)arr + (j * size);

      int result = comparator(jth, ith);
      if (result > 0) {
        ShiftArray(size, jth);
        open_index--;
      } else {
        break;
      }
    }

    memcpy((char*)arr + (open_index * size), ith, size);
    free(ith);
  }

  return 0;
}
