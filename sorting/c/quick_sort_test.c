#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "quick_sort.h"
#include "sorting_test_helpers.h"

/* Test Suite setup and cleanup functions: */
static int init_suite(void) { return 0; }
static int clean_suite(void) { return 0; }

/************* quick sort ****************/
void quick_sort_null()
{
  int result = quick_sort(1, 1, NULL, NULL);
  CU_ASSERT_EQUAL(-1, result);
}

void quick_sort_reverse_arry()
{
  const int n = 6;
  const int expected[] = { 1, 2, 3, 4, 5, 6 };
  int arr[] = { 6, 5, 4, 3, 2, 1 };

  int result = quick_sort(n, sizeof(arr[0]), arr, int_comparator);

  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
  CU_ASSERT_EQUAL(result, 0);
}

void quick_sort_standard()
{
  const int n = 6;
  const int expected[] = { 1, 2, 3, 4, 5, 6 };
  int arr[] = { 4, 2, 3, 1, 5, 6 };

  int result = quick_sort(n, sizeof(int), arr, int_comparator);

  CU_ASSERT_EQUAL(result, 0);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

void quick_sort_does_not_chage_sorted()
{
  const int n = 6;
  const int expected[] = { 1, 2, 3, 4, 5, 6 };
  int arr[] = { 1, 2, 3, 4, 5, 6 };

  int result = quick_sort(n, sizeof(int), arr, int_comparator);

  CU_ASSERT_EQUAL(result, 0);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

void quick_sort_dup_items()
{
  const int n = 6;
  const int expected[] = { 1, 1, 1, 2, 2, 2 };
  int arr[] = { 2, 1, 2, 1, 2, 1 };

  int result = quick_sort(n, sizeof(int), arr, int_comparator);

  CU_ASSERT_EQUAL(result, 0);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

void quick_sort_structs()
{
  const int n = 6;
  const test_struct_t expected[] = { { 0, 0, 1 }, { 0, 0, 2 }, { 0, 0, 3 },
    { 0, 0, 4 }, { 0, 0, 5 }, { 0, 0, 6 } };
  test_struct_t arr[] = { { 0, 0, 6 }, { 0, 0, 5 }, { 0, 0, 4 }, { 0, 0, 3 },
    { 0, 0, 2 }, { 0, 0, 1 } };

  int result = quick_sort(n, sizeof(arr[0]), arr, struct_comparator);

  CU_ASSERT_EQUAL(result, 0);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

void quick_sort_matches_c()
{
  const size_t n = 1000;
  int c_arr[n];
  int arr[n];

  for (size_t i = 0; i < n; i++) {
    int temp = rand();
    c_arr[i] = temp;
    arr[i] = temp;
  }

  qsort(c_arr, n, sizeof(c_arr[0]), int_comparator);
  quick_sort(n, sizeof(arr[0]), arr, int_comparator);
  arrays_are_equal(n, sizeof(c_arr[0]), c_arr, arr);
}

int quick_sort_suite()
{
  CU_pSuite pSuite = NULL;
  pSuite = CU_add_suite("quick sort suite", init_suite, clean_suite);
  if (NULL == pSuite) {
    return -1;
  }

  /* add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "null does not throw", quick_sort_null)
          || NULL == CU_add_test(pSuite, "small array", quick_sort_reverse_arry)
          || NULL == CU_add_test(pSuite, "standard case", quick_sort_standard)
          || NULL
              == CU_add_test(pSuite, "does not change pre-sorted array",
                     quick_sort_does_not_chage_sorted)
          || NULL
              == CU_add_test(
                     pSuite, "quick_sort matches c qsort", quick_sort_matches_c)
          || NULL
              == CU_add_test(pSuite, "quick_sort sorts duplicate items",
                     quick_sort_dup_items)
          || NULL
              == CU_add_test(
                     pSuite, "quick_sort sorts structs", quick_sort_structs)
          /* del this comment */
          )

  ) {
    return CU_get_error();
  }

  return 0;
}

/************* partition ****************/
void partition_null()
{
  int result = partition(0, 0, NULL, NULL, NULL);
  CU_ASSERT_EQUAL(-1, result);
}

void partition_does_not_chage_sorted()
{
  const int n = 6;
  const int expected[] = { 1, 2, 3, 4, 5, 6 };
  int arr[] = { 1, 2, 3, 4, 5, 6 };
  size_t pivot_index;

  int result = partition(n, sizeof(int), arr, int_comparator, &pivot_index);

  CU_ASSERT_EQUAL(result, 0);
  CU_ASSERT_EQUAL(pivot_index, 0);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

void partition_pivot_on_center()
{
  const int n = 6;
  const int expected[] = { 1, 2, 3, 4, 5, 6 };
  int arr[] = { 4, 2, 3, 1, 5, 6 };
  size_t pivot_index;

  int result = partition(n, sizeof(int), arr, int_comparator, &pivot_index);

  CU_ASSERT_EQUAL(result, 0);
  CU_ASSERT_EQUAL(pivot_index, 3);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

void partition_reverse_sorted()
{
  const int n = 6;
  const int expected[] = { 1, 5, 4, 3, 2, 6 };

  int arr[] = { 6, 5, 4, 3, 2, 1 };
  size_t pivot_index;

  int result = partition(n, sizeof(int), arr, int_comparator, &pivot_index);

  CU_ASSERT_EQUAL(result, 0);
  CU_ASSERT_EQUAL(pivot_index, 5);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

void partition_first_in_correct_position()
{
  const int n = 5;
  const int expected[] = { 1, 5, 4, 3, 2 };

  int arr[] = { 1, 5, 4, 3, 2 };
  size_t pivot_index;

  int result = partition(n, sizeof(int), arr, int_comparator, &pivot_index);

  CU_ASSERT_EQUAL(result, 0);
  CU_ASSERT_EQUAL(pivot_index, 0);
  arrays_are_equal(n, sizeof(expected[0]), expected, arr);
}

int partition_suite()
{
  CU_pSuite pSuite = NULL;
  pSuite = CU_add_suite("partition suite", init_suite, clean_suite);
  if (NULL == pSuite) {
    return -1;
  }

  /* add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "null does not throw", partition_null)
          || NULL
              == CU_add_test(pSuite,
                     "partition does not modify pre sorted array",
                     partition_does_not_chage_sorted)
          || NULL
              == CU_add_test(pSuite, "partition pivot on center item",
                     partition_pivot_on_center)
          || NULL
              == CU_add_test(pSuite, "partition pivot on reverse sorted array",
                     partition_reverse_sorted)
          || NULL
              == CU_add_test(pSuite,
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
