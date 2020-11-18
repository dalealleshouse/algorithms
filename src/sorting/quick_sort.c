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

static ResultCode Swap(const size_t kSize, void* x, void* y) {
  if (kSize == 0 || x == NULL || y == NULL) return kNullParameter;
  if (kSize == 0) return kArgumentOutOfRange;

  char n[kSize];
  memcpy(n, x, kSize);
  memcpy(x, y, kSize);
  memcpy(y, n, kSize);

  return 0;
}

int PivotOnZero(const size_t n, const size_t size, const void* arr,
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
int PivotOnRandom(const size_t n, const size_t size, const void* arr,
                  const sort_strategy comparator) {
  unsigned int seed = time(NULL);

  (void)size;
  (void)arr;
  (void)comparator;
  return rand_r(&seed) % (int)n;
}

int PivotOnLast(const size_t n, const size_t size, const void* arr,
                const sort_strategy comparator) {
  (void)size;
  (void)arr;
  (void)comparator;
  if (n <= 1) return 0;

  return (int)n - 1;
}

int PivotOnMedian(const size_t n, const size_t size, const void* arr,
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

// It is assumed that the pivot will be the first item in the array
ResultCode Partition(const size_t n, const size_t size, void* arr,
                     const sort_strategy comparator, size_t* pivot_index) {
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
      if (Swap(size, t_arr, pivot_pos) < 0) return -1;

      pivot_pos += size;
      (*pivot_index)++;
    }

    t_arr += size;
  }

  if (*pivot_index > 0) {
    ResultCode result_code =
        Swap(size, pivot_value, pivot_value + *pivot_index * size);
    if (result_code != kSuccess) return result_code;
  }

  return kSuccess;
}

ResultCode QuickSort(const size_t n, const size_t size, void* arr,
                     const sort_strategy comparator) {
  return QuickSortPivot(n, size, arr, comparator, PivotOnRandom);
}

ResultCode QuickSortPivot(const size_t n, const size_t size, void* arr,
                          const sort_strategy comparator,
                          const ChoosePivot choose_pivot) {
  if (arr == NULL || comparator == NULL) return kNullParameter;
  if (n == 0 || size == 0) return kArgumentOutOfRange;

  if (n <= 1) return 0;

  size_t pivot_index;
  int pivot;

  if ((pivot = choose_pivot(n, size, arr, comparator)) < 0) return -1;

  // move the partition value to the first position
  if (Swap(size, arr, (char*)arr + pivot * size) != 0) return -2;

  if (Partition(n, size, arr, comparator, &pivot_index) != 0) return -3;

  int result;
  // items to the left of the partition
  if (pivot_index > 0) {
    if ((result = QuickSortPivot(pivot_index, size, arr, comparator,
                                 choose_pivot)) != 0) {
      return result;
    }
  }

  // items to the right of the partition
  pivot_index++;
  if (pivot_index < n) {
    if ((result = QuickSortPivot(n - pivot_index, size,
                                 (char*)arr + pivot_index * size, comparator,
                                 choose_pivot)) != 0) {
      return result;
    }
  }

  return 0;
}
