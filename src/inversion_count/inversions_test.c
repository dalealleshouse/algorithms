/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "inversions.h"

#include <stdio.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "test_helpers.h"

typedef struct TestStruct {
  int foo;
  int bar;
  int sorter;
} TestStruct;

static int TestStructComparator(const void* x, const void* y) {
  if (x == y) return 0;
  TestStruct* _x = (TestStruct*)x;
  TestStruct* _y = (TestStruct*)y;

  return PIntComparator(&_x->sorter, &_y->sorter);
}

int PTestStructComparator(const void* x, const void* y) {
  if (x == y) return 0;
  TestStruct* _x = *(TestStruct**)x;
  TestStruct* _y = *(TestStruct**)y;

  return PIntComparator(&_x->sorter, &_y->sorter);
}

void CountInversions_null_parameter(void) {
  size_t result = 0;

  ResultCode result_code =
      CountInversions(NULL, 0, sizeof(int), PIntComparator, &result);
  CU_ASSERT_EQUAL(result_code, kNullParameter);
}

void CountInversions_invalid_size(void) {
  const int size = 4;
  int arr[] = {1, 2, 4, 3};
  size_t result = 0;

  ResultCode result_code =
      CountInversions(arr, size, 0, PIntComparator, &result);
  CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);

  result_code = CountInversions(arr, 0, sizeof(int), PIntComparator, &result);
  CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);
}

void CountInversions_count_single_inversion(void) {
  const int size = 4;
  int arr[] = {1, 2, 4, 3};

  size_t result = 0;
  ResultCode result_code =
      CountInversions(arr, size, sizeof(int), PIntComparator, &result);

  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(result, 1);
}

void CountInversions_count_two_inversion(void) {
  const int size = 4;
  int arr[] = {2, 1, 4, 3};

  size_t result = 0;
  ResultCode result_code =
      CountInversions(arr, size, sizeof(int), PIntComparator, &result);

  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(result, 2);
}

void CountInversions_count_reverse_sorted_array() {
  const int size = 6;
  int arr[] = {6, 5, 4, 3, 2, 1};

  size_t result = 0;
  ResultCode result_code =
      CountInversions(arr, size, sizeof(int), PIntComparator, &result);

  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(result, 15);
}

void CountInversions_count_odd_sized_array() {
  const int size = 5;
  int arr[] = {5, 4, 3, 2, 1};

  size_t result = 0;
  ResultCode result_code =
      CountInversions(arr, size, sizeof(int), PIntComparator, &result);

  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(result, 10);
}

void CountInversions_count_sorted_array() {
  const int size = 6;
  int arr[] = {1, 2, 3, 4, 5, 6};

  size_t result = 0;
  ResultCode result_code =
      CountInversions(arr, size, sizeof(int), PIntComparator, &result);

  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(result, 0);
}

void CountInversions_counts_structs(void) {
  const int size = 6;
  TestStruct arr[6] = {{0, 0, 6}, {0, 0, 5}, {0, 0, 4},
                       {0, 0, 3}, {0, 0, 2}, {0, 0, 1}};

  size_t result = 0;
  ResultCode result_code = CountInversions(arr, size, sizeof(TestStruct),
                                           TestStructComparator, &result);

  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(result, 15);
}

void CountInversions_sorts_pointers(void) {
  const int kSize = 6;
  TestStruct* arr[kSize];

  for (int i = 0; i < kSize; i++) {
    arr[i] = malloc(sizeof(TestStruct));
    arr[i]->sorter = kSize - i;
  }

  size_t result = 0;
  ResultCode result_code = CountInversions(arr, kSize, sizeof(TestStruct*),
                                           PTestStructComparator, &result);

  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(result, 15);

  for (int i = 0; i < kSize; i++) free(arr[i]);
}

void CountInversions_does_not_count_equal_items() {
  const int size = 5;
  int arr[] = {1, 3, 3, 3, 5};

  size_t result = 0;
  ResultCode result_code =
      CountInversions(arr, size, sizeof(int), PIntComparator, &result);

  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(result, 0);
}

void CountInversions_massive_array() {
  // Very little error checking here b/c this is just a test and I know what's
  // in the input file
  size_t n = 100000;
  size_t tracker = 0;
  int arr[n];
  char line[256];
  FILE* file = fopen("src/inversion_count/test_data/int_array.txt", "r");

  while (fgets(line, sizeof(line), file) != NULL) {
    arr[tracker] = (int)strtol(line, NULL, 10);
    tracker++;
  }

  fclose(file);

  size_t result = 0;
  ResultCode result_code =
      CountInversions(arr, n, sizeof(int), PIntComparator, &result);

  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_EQUAL(result, 2407905288);
}

int RegisterInversionCountTests() {
  CU_TestInfo inversion_tests[] = {
      CU_TEST_INFO(CountInversions_null_parameter),
      CU_TEST_INFO(CountInversions_invalid_size),
      CU_TEST_INFO(CountInversions_count_single_inversion),
      CU_TEST_INFO(CountInversions_count_two_inversion),
      CU_TEST_INFO(CountInversions_count_reverse_sorted_array),
      CU_TEST_INFO(CountInversions_count_odd_sized_array),
      CU_TEST_INFO(CountInversions_count_sorted_array),
      CU_TEST_INFO(CountInversions_counts_structs),
      CU_TEST_INFO(CountInversions_sorts_pointers),
      CU_TEST_INFO(CountInversions_massive_array),
      CU_TEST_INFO(CountInversions_does_not_count_equal_items),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Inversion Count",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = inversion_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
