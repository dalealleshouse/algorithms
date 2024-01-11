/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "bubble_sort.h"

#include <stdbool.h>
#include <stdlib.h>

#include "CUnit/CUnit.h"
#include "sorting_test_helpers.h"

STANDARD_SORTING_TESTS(BubbleSort)

static int CrazyOddFirstComparator(const void* x, const void* y) {
  int* _x = (int*)x;
  int* _y = (int*)y;

  bool x_is_even = (*_x % 2) == 0;
  bool y_is_even = (*_y % 2) == 0;

  if (x_is_even == true && y_is_even == false) return 1;
  if (x_is_even == false && y_is_even == true) return -1;

  return *_x - *_y;
}

static void BubbleSortTestCase() {
  int* data = GenerateTestData();

  ResultCode result_code =
      BubbleSort(test_data_n, sizeof(int), data, CrazyOddFirstComparator);

  CU_ASSERT_EQUAL(result_code, kSuccess);

  free(data);
}

int RegisterBubbleSortTestCase() {
  CU_TestInfo bubble_sort_test_case[] = {CU_TEST_INFO(BubbleSortTestCase),
                                         CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Bubble Sort Test Case",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = bubble_sort_test_case},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
