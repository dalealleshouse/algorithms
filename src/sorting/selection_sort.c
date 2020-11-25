/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "selection_sort.h"

#include <stdbool.h>
#include <string.h>

#include "sort_instrumentation.h"

static void Swap(const size_t kSize, void* x, void* y) {
  ++swap_count;

  char n[kSize];
  memcpy(n, x, kSize);
  memcpy(x, y, kSize);
  memcpy(y, n, kSize);
}

ResultCode SelectionSort(const size_t n, const size_t size, void* arr,
                         const sort_strategy comparator) {
  if (arr == NULL || comparator == NULL) return kNullParameter;
  if (n == 0 || size == 0) return kArgumentOutOfRange;

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
      Swap(size, ith, lowest);
    }
  }

  return 0;
}
