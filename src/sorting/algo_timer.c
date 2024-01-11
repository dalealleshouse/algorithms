/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "algo_timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bubble_sort.h"
#include "insertion_sort.h"
#include "merge_sort.h"
#include "quick_sort.h"
#include "selection_sort.h"
#include "sorting_test_helpers.h"

typedef int (*sorter)(const size_t n, const size_t size, void* arr,
                      const sort_strategy comparator);

static int merge_sort_adapter(const size_t n, const size_t size, void* arr,
                              const sort_strategy comparator) {
  char* tmp[n * size];
  int result = MergeSort(arr, tmp, n, size, comparator);

  return result;
}

static int quick_random_adapter(const size_t n, const size_t size, void* arr,
                                const sort_strategy comparator) {
  return QuickSortPivot(n, size, arr, comparator, PivotOnRandom);
}

static int quick_first_adapter(const size_t n, const size_t size, void* arr,
                               const sort_strategy comparator) {
  return QuickSortPivot(n, size, arr, comparator, PivotOnZero);
}

static int quick_last_adapter(const size_t n, const size_t size, void* arr,
                              const sort_strategy comparator) {
  return QuickSortPivot(n, size, arr, comparator, PivotOnLast);
}

static int quick_median_adapter(const size_t n, const size_t size, void* arr,
                                const sort_strategy comparator) {
  return QuickSortPivot(n, size, arr, comparator, PivotOnMedian);
}

static sorter get_sorter(const sort_algo algo) {
  switch (algo) {
    case C:
      return QuickSort;
    case BUBBLE:
      return BubbleSort;
    case INSERTION:
      return InsertionSort;
    case SELECTION:
      return SelectionSort;
    case QUICK:
      return QuickSort;
    case MERGE:
      return merge_sort_adapter;
    case QUICK_PIVOT_RANDOM:
      return quick_random_adapter;
    case QUICK_PIVOT_MEDIAN:
      return quick_median_adapter;
    case QUICK_PIVOT_FIRST:
      return quick_first_adapter;
    case QUICK_PIVOT_LAST:
      return quick_last_adapter;
    default: {
      int fprint_result = fprintf(stderr, "Invalid sorting algorithm\n");
      if (fprint_result < 0) perror("fprintf error");
      return NULL;
    }
  }
}

static ArrayGenerator get_arr_get(const arr_type type) {
  switch (type) {
    case RANDOM:
      return RandomArrayGenerator;
    case DUPLICATES:
      return DuplicateValueArrayGenerator;
    case REVERSED:
      return ReversedArrayGenerator;
    case SORTED:
      return SequencedArrayGenerator;
    default: {
      int fprint_result = fprintf(stderr, "Invalid array type\n");
      if (fprint_result < 0) perror("fprintf error");
      return NULL;
    }
  }
}

double sort_time(const size_t n, const arr_type type, const sort_algo algo) {
  sorter sorter = get_sorter(algo);
  if (sorter == NULL) return -1;

  ArrayGenerator ArrayGenerator = get_arr_get(type);
  if (ArrayGenerator == NULL) return -1;

  int* arr = ArrayGenerator(n);

  clock_t t = clock();
  int result = sorter(n, sizeof(int), arr, PIntComparator);
  t = clock() - t;

  free(arr);
  if (result < 0) return -1;

  double time = ((double)t) / CLOCKS_PER_SEC;

  return time;
}
