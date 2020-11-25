/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "sort_instrumentation.h"

#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "bubble_sort.h"
#include "insertion_sort.h"
#include "selection_sort.h"
#include "sorting_test_helpers.h"
#include "test_helpers.h"

typedef ResultCode (*sorter)(const size_t n, const size_t size, void* arr,
                             const sort_strategy comparator);

size_t swap_count = 0;
size_t compare_count = 0;

static int InstrumentedComparator(const void* x, const void* y) {
  int* _x = (int*)x;
  int* _y = (int*)y;

  ++compare_count;

  return *_x - *_y;
}

static void InstructionCounts(sorter sorter, char* name) {
  swap_count = 0;
  compare_count = 0;

  int* test_data = GenerateTestData();

  sorter(test_data_n, sizeof(int), test_data, InstrumentedComparator);
  printf("%s Sort swaps = %lu, comparions = %lu\n", name, swap_count,
         compare_count);

  free(test_data);
}

static void SortInstrumentation() {
  printf("\n");
  InstructionCounts(BubbleSort, "Bubble");
  InstructionCounts(InsertionSort, "Insertion");
  InstructionCounts(SelectionSort, "Selection");
}

int RegisterSortInstrumentationTestCase() {
  CU_TestInfo sort_instrumentation[] = {CU_TEST_INFO(SortInstrumentation),
                                        CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Sort Instrumentation",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = sort_instrumentation},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
