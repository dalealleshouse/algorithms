#include "./malloc_test_wrapper.h"
#include <stdbool.h>
#include <stdlib.h>
#include "./test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

static void _MallocWorksWithoutIntercept() {
  void* test = malloc(100);
  CU_ASSERT_PTR_NOT_NULL(test);
  free(test);
}

static void _MallocInterceptReturnNullAndResetReturnsPointer() {
#if !defined(NDEBUG)
  InterceptMalloc();
  void* test = malloc(100);
  CU_ASSERT_PTR_NULL(test);

  ResetMalloc();
  test = malloc(100);
  CU_ASSERT_PTR_NOT_NULL(test);
  free(test);
#endif
}

static void _MallocInterceptCountIncrementsAndResets() {
#if !defined(NDEBUG)
  CU_ASSERT_EQUAL(0, MallocInterceptCount());

  InterceptMalloc();
  void* test = malloc(100);
  CU_ASSERT_EQUAL(1, MallocInterceptCount());
  test = malloc(100);
  CU_ASSERT_EQUAL(2, MallocInterceptCount());
  test = malloc(100);
  CU_ASSERT_EQUAL(3, MallocInterceptCount());

  ResetMalloc();
  CU_ASSERT_EQUAL(0, MallocInterceptCount());
#endif
}

static void _CallocWorksWithoutIntercept() {
  void* test = calloc(100, 1);
  CU_ASSERT_PTR_NOT_NULL(test);
  free(test);
}

static void _CallocInterceptReturnNullAndResetReturnsPointer() {
#if !defined(NDEBUG)
  InterceptMalloc();
  void* test = calloc(100, 1);
  CU_ASSERT_PTR_NULL(test);

  ResetMalloc();
  test = calloc(100, 1);
  CU_ASSERT_PTR_NOT_NULL(test);
  free(test);
#endif
}

static void _CallocInterceptCountIncrementsAndResets() {
#if !defined(NDEBUG)
  CU_ASSERT_EQUAL(0, MallocInterceptCount());

  InterceptMalloc();
  void* test = calloc(100, 1);
  CU_ASSERT_EQUAL(1, MallocInterceptCount());
  test = calloc(100, 1);
  CU_ASSERT_EQUAL(2, MallocInterceptCount());
  test = calloc(100, 1);
  CU_ASSERT_EQUAL(3, MallocInterceptCount());

  ResetMalloc();
  CU_ASSERT_EQUAL(0, MallocInterceptCount());
#endif
}

static void _ReallocWorksWithoutIntercept() {
  void* test = malloc(100);
  test = realloc(test, 1);
  CU_ASSERT_PTR_NOT_NULL(test);
  free(test);
}

static void _ReallocInterceptReturnNullAndResetReturnsPointer() {
#if !defined(NDEBUG)
  void* test = malloc(100);

  InterceptMalloc();
  void* test2 = realloc(test, 1);
  CU_ASSERT_PTR_NULL(test2);

  ResetMalloc();

  test = realloc(test, 1);
  CU_ASSERT_PTR_NOT_NULL(test);
  free(test);
#endif
}

static void _ReallocInterceptCountIncrementsAndResets() {
#if !defined(NDEBUG)
  void* test = malloc(100);

  CU_ASSERT_EQUAL(0, MallocInterceptCount());

  InterceptMalloc();
  void* test2 = realloc(test, 1);
  CU_ASSERT_EQUAL(1, MallocInterceptCount());
  test2 = realloc(test, 1);
  CU_ASSERT_EQUAL(2, MallocInterceptCount());
  test2 = realloc(test, 1);
  CU_ASSERT_EQUAL(3, MallocInterceptCount());

  ResetMalloc();
  CU_ASSERT_EQUAL(0, MallocInterceptCount());
  free(test);
#endif
}

int RegisterMallocTestWrapperTests() {
  CU_TestInfo tests[] = {
      CU_TEST_INFO(_MallocWorksWithoutIntercept),
      CU_TEST_INFO(_MallocInterceptReturnNullAndResetReturnsPointer),
      CU_TEST_INFO(_MallocInterceptCountIncrementsAndResets),
      CU_TEST_INFO(_CallocWorksWithoutIntercept),
      CU_TEST_INFO(_CallocInterceptReturnNullAndResetReturnsPointer),
      CU_TEST_INFO(_CallocInterceptCountIncrementsAndResets),
      CU_TEST_INFO(_ReallocWorksWithoutIntercept),
      CU_TEST_INFO(_ReallocInterceptReturnNullAndResetReturnsPointer),
      CU_TEST_INFO(_ReallocInterceptCountIncrementsAndResets),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "size_t_tests",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
