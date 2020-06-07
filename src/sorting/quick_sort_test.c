#include "./quick_sort.h"

#include <stdlib.h>
#include <unistd.h>

#include "../utils/test_helpers.h"
#include "./sorting_test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

/************* quick sort ****************/
const static size_t n = 200;
const static size_t test_data_n = 10000;

size_t comparisons = 0;

STANDARD_SORTING_TESTS(quick_sort)

static void large() { test_int_array(100000, rev_arr); }

static int* gen_test_data() {
  int* arr = malloc(sizeof(int) * test_data_n);
  FILE* file;

  file = fopen("src/sorting/test_data/quick_sort.txt", "r");

  size_t index = 0;
  int i = 0;

  fscanf(file, "%d", &i);
  while (!feof(file)) {
    arr[index] = i;
    index++;
    fscanf(file, "%d", &i);
  }
  fclose(file);

  return arr;
}

static void _test_pivot(choose_pivot choose_pivot) {
  int* arr = gen_test_data();
  int* arr_cpy = duplicate(arr, sizeof(int) * test_data_n);

  int result = quick_sort_pivot(test_data_n, sizeof(int), arr, PIntComparator,
                                choose_pivot);

  qsort(arr_cpy, test_data_n, sizeof(int), PIntComparator);

  CU_ASSERT_EQUAL_FATAL(0, result);
  arrays_are_equal(test_data_n, sizeof(int), arr_cpy, arr);
  free(arr);
  free(arr_cpy);
}

static void piviot_on_zero_matches_c() { _test_pivot(pivot_on_zero); }

static void piviot_on_random_matches_c() { _test_pivot(pivot_on_random); }

static void piviot_on_last_matches_c() { _test_pivot(pivot_on_last); }

static void piviot_on_median_matches_c() { _test_pivot(pivot_on_median); }

static void pivot_on_median_finds_first() {
  const int n = 6;
  const int expected = 0;
  const int arr[] = {3, 1, 2, 4, 5, 6};

  const int result = pivot_on_median(n, sizeof(int), arr, PIntComparator);

  CU_ASSERT_EQUAL(expected, result);
}

static void pivot_on_median_finds_last() {
  const int n = 6;
  const int expected = 5;
  const int arr[] = {6, 1, 2, 4, 5, 3};

  const int result = pivot_on_median(n, sizeof(int), arr, PIntComparator);

  CU_ASSERT_EQUAL(expected, result);
}

static void pivot_on_median_finds_middle() {
  const int n = 6;
  const int expected = 2;
  const int arr[] = {6, 1, 3, 4, 5, 2};

  const int result = pivot_on_median(n, sizeof(int), arr, PIntComparator);

  CU_ASSERT_EQUAL(expected, result);
}

int RegisterQuickSortTests() {
  CU_pSuite pSuite = NULL;
  pSuite = CU_add_suite("quick sort suite", noop, noop);
  if (NULL == pSuite) {
    return -1;
  }

  if (ADD_STANDARD_TESTS(pSuite) ||
      NULL == CU_add_test(pSuite, "large", large) ||
      NULL == CU_add_test(pSuite, "pivot on zero", piviot_on_zero_matches_c) ||
      NULL ==
          CU_add_test(pSuite, "pivot on random", piviot_on_random_matches_c) ||
      NULL == CU_add_test(pSuite, "pivot on last", piviot_on_last_matches_c) ||
      NULL ==
          CU_add_test(pSuite, "pivot on median", piviot_on_median_matches_c) ||
      NULL == CU_add_test(pSuite, "pivot on median finds first",
                          pivot_on_median_finds_first) ||
      NULL == CU_add_test(pSuite, "pivot on median finds last",
                          pivot_on_median_finds_last) ||
      NULL == CU_add_test(pSuite, "pivot on median finds middle",
                          pivot_on_median_finds_middle)) {
    return CU_get_error();
  }

  return 0;
}

/************* partition ****************/
static void partition_null() {
  int result = partition(0, 0, NULL, NULL, NULL);
  CU_ASSERT_EQUAL(-1, result);
}

static void partition_does_not_chage_sorted() {
  const int n = 6;
  const int expected[] = {1, 2, 3, 4, 5, 6};
  int arr[] = {1, 2, 3, 4, 5, 6};
  size_t pivot_index;

  int result = partition(n, sizeof(int), arr, PIntComparator, &pivot_index);

  CU_ASSERT_EQUAL(result, 0);
  CU_ASSERT_EQUAL(pivot_index, 0);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

static void partition_pivot_on_center() {
  const int n = 6;
  const int expected[] = {1, 2, 3, 4, 5, 6};
  int arr[] = {4, 2, 3, 1, 5, 6};
  size_t pivot_index;

  int result = partition(n, sizeof(int), arr, PIntComparator, &pivot_index);

  CU_ASSERT_EQUAL(result, 0);
  CU_ASSERT_EQUAL(pivot_index, 3);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

static void partition_reverse_sorted() {
  const int n = 6;
  const int expected[] = {1, 5, 4, 3, 2, 6};

  int arr[] = {6, 5, 4, 3, 2, 1};
  size_t pivot_index;

  int result = partition(n, sizeof(int), arr, PIntComparator, &pivot_index);

  CU_ASSERT_EQUAL(result, 0);
  CU_ASSERT_EQUAL(pivot_index, 5);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

static void partition_first_in_correct_position() {
  const int n = 5;
  const int expected[] = {1, 5, 4, 3, 2};

  int arr[] = {1, 5, 4, 3, 2};
  size_t pivot_index;

  int result = partition(n, sizeof(int), arr, PIntComparator, &pivot_index);

  CU_ASSERT_EQUAL(result, 0);
  CU_ASSERT_EQUAL(pivot_index, 0);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

int RegisterPartitionTests() {
  CU_pSuite pSuite = NULL;
  pSuite = CU_add_suite("partition suite", noop, noop);
  if (NULL == pSuite) {
    return -1;
  }

  /* add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "null does not throw", partition_null) ||
       NULL == CU_add_test(pSuite, "partition does not modify pre sorted array",
                           partition_does_not_chage_sorted) ||
       NULL == CU_add_test(pSuite, "partition pivot on center item",
                           partition_pivot_on_center) ||
       NULL == CU_add_test(pSuite, "partition pivot on reverse sorted array",
                           partition_reverse_sorted) ||
       NULL == CU_add_test(pSuite,
                           "partition does not change when first item is in "
                           "correct posoition",
                           partition_first_in_correct_position)
       /* del this comment */
       )

  ) {
    return CU_get_error();
  }

  return 0;
}
