#include "./inversions.h"
#include <stdio.h>
#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

typedef struct test_struct {
  int foo;
  int bar;
  int sorter;
} test_struct_t;

static int TestStructComparator(const void* x, const void* y) {
  if (x == y) return 0;

  if (x == NULL && y != NULL) return 0;

  if (y == NULL && x != NULL) return 1;

  test_struct_t* _x = (test_struct_t*)x;
  test_struct_t* _y = (test_struct_t*)y;

  return PIntComparator(&_x->sorter, &_y->sorter);
}

int PTestStructComparator(const void* x, const void* y) {
  if (x == y) return 0;

  if (x == NULL && y != NULL) return 0;

  if (y == NULL && x != NULL) return 1;

  test_struct_t* _x = *(test_struct_t**)x;
  test_struct_t* _y = *(test_struct_t**)y;

  return PIntComparator(&_x->sorter, &_y->sorter);
}

/************* Test case functions ****************/

void null_does_not_throw_test(void) {
  count_inversions(NULL, 0, sizeof(int), PIntComparator);
}

void count_single_inversion(void) {
  const int size = 4;
  int arr[] = {1, 2, 4, 3};

  int result = count_inversions(arr, size, sizeof(int), PIntComparator);

  CU_ASSERT_EQUAL(result, 1);
}

void count_two_inversion(void) {
  const int size = 4;
  int arr[] = {2, 1, 4, 3};

  int result = count_inversions(arr, size, sizeof(int), PIntComparator);

  CU_ASSERT_EQUAL(result, 2);
}

void count_reverse_sorted_array() {
  const int size = 6;
  int arr[] = {6, 5, 4, 3, 2, 1};

  int result = count_inversions(arr, size, sizeof(int), PIntComparator);

  CU_ASSERT_EQUAL(result, 15);
}

void count_odd_sized_array() {
  const int size = 5;
  int arr[] = {5, 4, 3, 2, 1};

  int result = count_inversions(arr, size, sizeof(int), PIntComparator);

  CU_ASSERT_EQUAL(result, 10);
}

void count_sorted_array() {
  const int size = 6;
  int arr[] = {1, 2, 3, 4, 5, 6};

  int result = count_inversions(arr, size, sizeof(int), PIntComparator);

  CU_ASSERT_EQUAL(result, 0);
}

void counts_structs(void) {
  const int size = 6;
  test_struct_t arr[6] = {{0, 0, 6}, {0, 0, 5}, {0, 0, 4},
                          {0, 0, 3}, {0, 0, 2}, {0, 0, 1}};

  int result =
      count_inversions(arr, size, sizeof(test_struct_t), TestStructComparator);

  CU_ASSERT_EQUAL(result, 15);
}

void sorts_pointers(void) {
  const int kSize = 6;
  test_struct_t* arr[kSize];

  for (int i = 0; i < kSize; i++) {
    arr[i] = malloc(sizeof(test_struct_t));
    arr[i]->sorter = kSize - i;
  }

  int result = count_inversions(arr, kSize, sizeof(test_struct_t*),
                                PTestStructComparator);

  CU_ASSERT_EQUAL(result, 15);

  for (int i = 0; i < kSize; i++) free(arr[i]);
}

void does_not_count_equal_items() {
  const int size = 5;
  int arr[] = {1, 3, 3, 3, 5};

  int result = count_inversions(arr, size, sizeof(int), PIntComparator);

  CU_ASSERT_EQUAL(result, 0);
}

void massive_array() {
  // Very little error checking here b/c this is just a test and I know what's
  // in the input file
  size_t n = 100000;
  size_t tracker = 0;
  int arr[n];
  char line[256];
  FILE* file = fopen("src/inversion_count/test_data/int_array.txt", "r");

  while (fgets(line, sizeof(line), file) != NULL) {
    arr[tracker] = atoi(line);
    tracker++;
  }

  fclose(file);

  unsigned long result = count_inversions(arr, n, sizeof(int), PIntComparator);

  CU_ASSERT_EQUAL(result, 2407905288);
}

int RegisterInversionCountTests() {
  CU_TestInfo inversion_tests[] = {CU_TEST_INFO(null_does_not_throw_test),
                                   CU_TEST_INFO(count_single_inversion),
                                   CU_TEST_INFO(count_two_inversion),
                                   CU_TEST_INFO(count_reverse_sorted_array),
                                   CU_TEST_INFO(count_odd_sized_array),
                                   CU_TEST_INFO(count_sorted_array),
                                   CU_TEST_INFO(counts_structs),
                                   CU_TEST_INFO(sorts_pointers),
                                   CU_TEST_INFO(massive_array),
                                   CU_TEST_INFO(does_not_count_equal_items),
                                   CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Inversion Count",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = inversion_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
