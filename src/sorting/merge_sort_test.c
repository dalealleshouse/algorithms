/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "merge_sort.h"

#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "sorting_test_helpers.h"

static ResultCode MergeSortAdapter(const size_t n, const size_t size, void* arr,
                                   const comparator comparator) {
  if (arr == NULL || comparator == NULL || n == 0 || size == 0) {
    return MergeSort(arr, arr, n, size, comparator);
  }

  char* tmp[n * size];
  memcpy(tmp, arr, n * size);
  return MergeSort(tmp, arr, n, size, comparator);
}

STANDARD_SORTING_TESTS(MergeSortAdapter)
