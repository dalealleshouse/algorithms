/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "merge_sort.h"

#include <alloca.h>
#include <stdio.h>
#include <string.h>

#include "sort_instrumentation.h"

ResultCode MergeSort(const void* arr, void* output, const size_t n,
                     const size_t item_size, sort_strategy comparator) {
  if (arr == NULL || output == NULL) return kNullParameter;
  if (n == 0 || item_size == 0) return kArgumentOutOfRange;

  // If n is odd, this will assign the larger half to b.
  size_t a_n = n / 2;
  size_t b_n = n - a_n;

  size_t a_size = a_n * item_size;
  size_t b_size = b_n * item_size;

  char* a;
  char* b;

  if (a_n == 1) {
    a = (char*)arr;
  } else {
    a = alloca(a_size);
    MergeSort(arr, a, a_n, item_size, comparator);
  }

  if (b_n == 1) {
    b = (char*)arr + a_size;
  } else {
    b = alloca(b_size);
    MergeSort((char*)arr + a_size, b, b_n, item_size, comparator);
  }

  size_t a_pos = 0;
  size_t b_pos = 0;
  for (size_t i = 0; i < n; i++) {
    if (a_pos >= a_size) {
      INSTRUMENTED_MEMCPY(output, &b[b_pos], b_size - b_pos, item_size);
      break;
    } else if (b_pos >= b_size) {
      INSTRUMENTED_MEMCPY(output, &a[a_pos], a_size - a_pos, item_size);
      break;
    } else {
      int result = comparator(&a[a_pos], &b[b_pos]);

      if (result < 0) {
        INSTRUMENTED_MEMCPY(output, &a[a_pos], item_size, item_size);
        a_pos += item_size;
      } else {
        INSTRUMENTED_MEMCPY(output, &b[b_pos], item_size, item_size);
        b_pos += item_size;
      }
    }

    output = (char*)output + item_size;
  }

  return kSuccess;
}

ResultCode MergeSortAdapter(const size_t n, const size_t size, void* arr,
                            const sort_strategy comparator) {
  if (arr == NULL || comparator == NULL || n == 0 || size == 0) {
    return MergeSort(arr, arr, n, size, comparator);
  }

  char* tmp[n * size];
  memcpy(tmp, arr, n * size);
  return MergeSort(tmp, arr, n, size, comparator);
}
