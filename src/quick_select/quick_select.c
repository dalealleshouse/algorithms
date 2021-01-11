/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "quick_select.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "quick_sort.h"

typedef size_t (*choose_select_pivot)(const size_t n);

static void Swap(const size_t size, void* x, void* y) {
  char n[size];
  memcpy(n, x, size);
  memcpy(x, y, size);
  memcpy(y, n, size);
}

static ResultCode QuickSelectRecursive(const size_t nth, const size_t n,
                                       const size_t size, void* arr,
                                       const sort_strategy comparator,
                                       void** result) {
  if (n <= 1) {
    *result = arr;
    return kSuccess;
  }

  // find the index of an element to partition around
  size_t pivot = random() % n;

  // move the partition value to the first position
  Swap(size, arr, (char*)arr + pivot * size);

  // partition around the first element
  size_t pivot_index;
  ResultCode result_code = Partition(n, size, arr, comparator, &pivot_index);
  if (result_code != kSuccess) return result_code;

  if (pivot_index == nth) {
    *result = (char*)arr + pivot_index * size;
    return kSuccess;
  } else if (pivot_index < nth) {
    return QuickSelect(nth - pivot_index, n - pivot_index, size,
                       (char*)arr + pivot_index * size, comparator, result);
  }

  return QuickSelect(nth, pivot_index, size, arr, comparator, result);
}

ResultCode QuickSelect(const size_t nth, const size_t n, const size_t size,
                       void* arr, const sort_strategy comparator,
                       void** result) {
  if (arr == NULL || comparator == NULL || result == NULL) {
    return kNullParameter;
  }

  if (nth > n - 1) return kArgumentOutOfRange;
  if (*result != NULL) return kOutputPointerIsNotNull;

  return QuickSelectRecursive(nth, n, size, arr, comparator, result);
}

ResultCode SortSelect(const size_t nth, const size_t n, const size_t size,
                      void* arr, const sort_strategy comparator,
                      void** result) {
  if (arr == NULL || comparator == NULL) return kNullParameter;
  if (nth > n - 1) return kArgumentOutOfRange;
  if (*result != NULL) return kOutputPointerIsNotNull;

  if (n <= 1) {
    *result = arr;
    return kSuccess;
  }

  qsort(arr, n, size, comparator);
  *result = (char*)arr + nth * size;
  return kSuccess;
}
