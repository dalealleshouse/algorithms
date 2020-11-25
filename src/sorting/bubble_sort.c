/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "bubble_sort.h"

#include <stdbool.h>
#include <string.h>

#include "sort_instrumentation.h"

static void Swap(const size_t size, void* x, void* y) {
  char n[size];
  memcpy(n, x, size);
  memcpy(x, y, size);
  memcpy(y, n, size);

  ++swap_count;
}

ResultCode BubbleSort(const size_t n, const size_t size, void* arr,
                      const sort_strategy comparator) {
  if (arr == NULL || comparator == NULL) return kNullParameter;
  if (n == 0 || size == 0) return kArgumentOutOfRange;

  size_t unsorted_to = n - 1;
  bool sorted = false;

  while (!sorted) {
    sorted = true;

    for (size_t i = 0; i < unsorted_to; i++) {
      void* ith = (char*)arr + (i * size);
      void* ith_1 = (char*)arr + (i * size) + size;

      int result = comparator(ith, ith_1);

      if (result > 0) {
        Swap(size, ith, ith_1);
        sorted = false;
      }
    }

    unsorted_to--;
  }

  return 0;
}
