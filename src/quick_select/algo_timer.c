/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "algo_timer.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "quick_select.h"

typedef ResultCode (*selector)(const size_t nth, const size_t n,
                               const size_t size, void* arr,
                               const sort_strategy comparator, void** result);

static selector GetSelector(const SelectAlgorithm algo) {
  switch (algo) {
    case kQuickSelect:
      return QuickSelect;
    case kSortSelect:
      return SortSelect;
    default:
      return NULL;
  }
}

double SelectTime(const size_t n, const size_t nth,
                  const SelectAlgorithm algo) {
  selector selector = GetSelector(algo);
  if (selector == NULL) {
    fprintf(stderr, "Invalid select algorithm, %d\n", algo);
    return -1;
  }

  int* arr = malloc(n * sizeof(int));
  for (size_t i = 0; i < n; i++) arr[i] = (int)(random() % INT_MAX);

  clock_t t = clock();
  void* result = NULL;
  ResultCode result_code =
      selector(nth, n, sizeof(int), arr, PIntComparator, &result);
  t = clock() - t;

  free(arr);

  if (result_code != kSuccess) {
    fprintf(stderr, "Selector returned an error code: %s",
            Result_ErrorMessage(result_code));
    return -1;
  }

  double time = ((double)t) / CLOCKS_PER_SEC;

  return time;
}
