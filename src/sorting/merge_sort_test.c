#include "./merge_sort.h"

#include <stdlib.h>  // for printf

#include "./sorting_test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

const static size_t n = 200;

/* Test Suite setup and cleanup functions: */
static int init_suite(void) { return 0; }
static int clean_suite(void) { return 0; }

static int merge_sort_test_adapter(const size_t n, const size_t size, void* arr,
                                   const comparator comparator) {
  if (arr == NULL) return merge_sort(arr, arr, n, size, comparator);

  char* tmp[n * size];
  memcpy(tmp, arr, n * size);
  int result = merge_sort(tmp, arr, n, size, comparator);

  return result;
}

STANDARD_SORTING_TESTS(merge_sort_test_adapter)

int RegisterMergeSortTestTests() {
  CU_pSuite pSuite = NULL;
  pSuite = CU_add_suite("merge sort suite", init_suite, clean_suite);
  if (NULL == pSuite) return -1;

  if (ADD_STANDARD_TESTS(pSuite)) return CU_get_error();

  return 0;
}
