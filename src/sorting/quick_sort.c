/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "quick_sort.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sort_instrumentation.h"

static void Swap(const size_t size, void* x, void* y) {
  char n[size];
  INSTRUMENTED_MEMCPY(n, x, size, size);
  INSTRUMENTED_MEMCPY(x, y, size, size);
  INSTRUMENTED_MEMCPY(y, n, size, size);
}

size_t PivotOnZero(const size_t n, const size_t size, const void* arr,
                   const sort_strategy comparator) {
  (void)n;
  (void)size;
  (void)arr;
  (void)comparator;
  return 0;
}

/*
 * This is a flawed implementation because it should be returning a uniformly
 * random number and it's not. However, for the purposes of this project, it's
 * not that important
 */
size_t PivotOnRandom(const size_t n, const size_t size, const void* arr,
                     const sort_strategy comparator) {
  unsigned int seed = time(NULL);

  (void)size;
  (void)arr;
  (void)comparator;
  return rand_r(&seed) % (int)n;
}

size_t PivotOnLast(const size_t n, const size_t size, const void* arr,
                   const sort_strategy comparator) {
  (void)size;
  (void)arr;
  (void)comparator;
  if (n <= 1) return 0;

  return (int)n - 1;
}

size_t PivotOnMedian(const size_t n, const size_t size, const void* arr,
                     const sort_strategy comparator) {
  if (n <= 2) return 0;

  size_t mid_point = 0;

  if (n % 2 == 0) {
    mid_point = n / 2 - 1;
  } else {
    mid_point = n / 2;
  }

  const void* first = arr;
  const void* last = (char*)arr + size * (n - 1);
  const void* middle = (char*)arr + size * mid_point;

  // first
  if ((comparator(first, last) >= 0 && comparator(first, middle) <= 0) ||
      (comparator(first, last) <= 0 && comparator(first, middle) >= 0)) {
    return 0;
  }

  // middle
  if ((comparator(middle, first) >= 0 && comparator(middle, last) <= 0) ||
      (comparator(middle, first) <= 0 && comparator(middle, last) >= 0)) {
    return mid_point;
  }

  // the only choice left is last
  return (int)n - 1;
}

static void* calc_pointer(void* arr, const size_t size, size_t index) {
  return (char*)arr + size * index;
}

// It is assumed that the pivot will be the first item in the array
ResultCode Partition(const size_t n, const size_t size, void* arr,
                     const sort_strategy comparator, size_t* pivot_index) {
  if (arr == NULL || comparator == NULL) return kNullParameter;
  if (n <= 1 || size == 0) return kArgumentOutOfRange;

  char* pivot_value = arr;
  size_t low = 1;
  size_t high = n - 1;

  while (true) {
    while (low < high &&
           comparator(calc_pointer(arr, size, low), pivot_value) < 0) {
      low++;
    }

    while (high >= low &&
           comparator(calc_pointer(arr, size, high), pivot_value) >= 0) {
      high--;
    }

    if (low >= high) break;
    Swap(size, calc_pointer(arr, size, low), calc_pointer(arr, size, high));
    low++;
    high--;
  }

  // Move the pivot into the correct place
  Swap(size, arr, calc_pointer(arr, size, high));
  *pivot_index = high;

  return kSuccess;
}

ResultCode InefficentPartition(const size_t n, const size_t size, void* arr,
                               const sort_strategy comparator,
                               size_t* pivot_index) {
  if (arr == NULL || comparator == NULL) return kNullParameter;
  if (n == 0 || size == 0) return kArgumentOutOfRange;

  char* t_arr = (char*)arr;
  char* pivot_value = t_arr;
  t_arr += size;
  char* pivot_pos = t_arr;
  *pivot_index = 0;

  for (size_t i = 1; i < n; i++) {
    // If the item is less, swap it, otherwise do nothing
    if (comparator(t_arr, pivot_value) < 0) {
      Swap(size, t_arr, pivot_pos);

      pivot_pos += size;
      (*pivot_index)++;
    }

    t_arr += size;
  }

  if (*pivot_index > 0) {
    Swap(size, pivot_value, pivot_value + *pivot_index * size);
  }

  return kSuccess;
}

ResultCode QuickSort(const size_t n, const size_t size, void* arr,
                     const sort_strategy comparator) {
  return QuickSortPivot(n, size, arr, comparator, PivotOnRandom);
}

ResultCode QuickSortPivot(const size_t n, const size_t size, void* arr,
                          const sort_strategy comparator,
                          const choose_pivot choose_pivot) {
  if (arr == NULL || comparator == NULL) return kNullParameter;
  if (n == 0 || size == 0) return kArgumentOutOfRange;

  if (n <= 1) return kSuccess;

  size_t pivot_index;
  size_t pivot;

  pivot = choose_pivot(n, size, arr, comparator);

  // move the partition value to the first position
  Swap(size, arr, (char*)arr + pivot * size);

  // This will move the item at index 0 to it's final placement
  ResultCode result_code = Partition(n, size, arr, comparator, &pivot_index);
  if (result_code != kSuccess) return result_code;

  // items to the left of the partition
  // The pivot index is zero based, so pivot_index is equivalent to n - 1
  if (pivot_index > 0) {
    if ((result_code = QuickSortPivot(pivot_index, size, arr, comparator,
                                      choose_pivot)) != kSuccess) {
      return result_code;
    }
  }

  // items to the right of the partition
  pivot_index++;
  if (pivot_index < n) {
    if ((result_code = QuickSortPivot(n - pivot_index, size,
                                      (char*)arr + pivot_index * size,
                                      comparator, choose_pivot)) != kSuccess) {
      return result_code;
    }
  }

  return kSuccess;
}
