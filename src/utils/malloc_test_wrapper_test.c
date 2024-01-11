#include <stdbool.h>
#include <stdlib.h>

#include "CUnit/CUnit.h"
#include "test_helpers.h"
#if !defined(NDEBUG)
#include "malloc_test_wrapper.h"
#endif

static void MallocWorksWithoutIntercept() {
  void* test = malloc(100);
  CU_ASSERT_PTR_NOT_NULL(test);
  free(test);
}

static void MallocInterceptReturnNullAndResetReturnsPointer() {
#if !defined(NDEBUG)
  InterceptMalloc();
  void* test = malloc(100);
  CU_ASSERT_PTR_NULL(test);

  ResetMalloc();
  test = malloc(100);
  CU_ASSERT_PTR_NOT_NULL(test);  // NOLINT(clang-analyzer-unix.Malloc)
  free(test);
#endif
}

static void MallocInterceptCountIncrementsAndResets() {
#if !defined(NDEBUG)
  CU_ASSERT_EQUAL(0, MallocInterceptCount());

  InterceptMalloc();
  void* test = malloc(100);
  CU_ASSERT_EQUAL(1, MallocInterceptCount());
  free(test);
  test = malloc(100);
  CU_ASSERT_EQUAL(2, MallocInterceptCount());
  free(test);
  test = malloc(100);
  CU_ASSERT_EQUAL(3, MallocInterceptCount());
  free(test);

  ResetMalloc();
  CU_ASSERT_EQUAL(0, MallocInterceptCount());
#endif
}

static void CallocWorksWithoutIntercept() {
  void* test = calloc(100, 1);
  CU_ASSERT_PTR_NOT_NULL(test);
  free(test);
}

static void CallocInterceptReturnNullAndResetReturnsPointer() {
#if !defined(NDEBUG)
  InterceptMalloc();
  void* test = calloc(100, 1);
  CU_ASSERT_PTR_NULL(test);

  ResetMalloc();
  test = calloc(100, 1);
  CU_ASSERT_PTR_NOT_NULL(test);  // NOLINT(clang-analyzer-unix.Malloc)
  free(test);
#endif
}

static void CallocInterceptCountIncrementsAndResets() {
#if !defined(NDEBUG)
  CU_ASSERT_EQUAL(0, MallocInterceptCount());

  InterceptMalloc();
  void* test = calloc(100, 1);
  CU_ASSERT_EQUAL(1, MallocInterceptCount());
  free(test);

  test = calloc(100, 1);
  CU_ASSERT_EQUAL(2, MallocInterceptCount());
  free(test);

  test = calloc(100, 1);
  CU_ASSERT_EQUAL(3, MallocInterceptCount());
  free(test);

  ResetMalloc();
  CU_ASSERT_EQUAL(0, MallocInterceptCount());
#endif
}

static void ReallocWorksWithoutIntercept() {
  void* test = malloc(100);
  test = realloc(test, 1);
  CU_ASSERT_PTR_NOT_NULL(test);
  free(test);  // NOLINT(clang-analyzer-unix.Malloc)
}

static void ReallocInterceptReturnNullAndResetReturnsPointer() {
#if !defined(NDEBUG)
  void* test = malloc(100);

  InterceptMalloc();
  void* test2 = realloc(test, 1);
  CU_ASSERT_PTR_NULL(test2);

  ResetMalloc();

  test = realloc(test, 1);  // NOLINT(clang-analyzer-unix.Malloc)
  CU_ASSERT_PTR_NOT_NULL(test);
  free(test);  // NOLINT(clang-analyzer-unix.Malloc)
#endif
}

static void ReallocInterceptCountIncrementsAndResets() {
#if !defined(NDEBUG)
  void* test = malloc(100);

  CU_ASSERT_EQUAL(0, MallocInterceptCount());

  InterceptMalloc();
  void* test2 = realloc(test, 1);
  CU_ASSERT_EQUAL(1, MallocInterceptCount());
  free(test2);

  void* test3 = realloc(test, 1);  // NOLINT(clang-analyzer-unix.Malloc)
  CU_ASSERT_EQUAL(2, MallocInterceptCount());
  free(test3);

  test2 = realloc(test, 1);
  CU_ASSERT_EQUAL(3, MallocInterceptCount());
  free(test2);

  ResetMalloc();
  CU_ASSERT_EQUAL(0, MallocInterceptCount());
  free(test);
#endif
}

int RegisterMallocTestWrapperTests() {
  CU_TestInfo tests[] = {
      CU_TEST_INFO(MallocWorksWithoutIntercept),
      CU_TEST_INFO(MallocInterceptReturnNullAndResetReturnsPointer),
      CU_TEST_INFO(MallocInterceptCountIncrementsAndResets),
      CU_TEST_INFO(CallocWorksWithoutIntercept),
      CU_TEST_INFO(CallocInterceptReturnNullAndResetReturnsPointer),
      CU_TEST_INFO(CallocInterceptCountIncrementsAndResets),
      CU_TEST_INFO(ReallocWorksWithoutIntercept),
      CU_TEST_INFO(ReallocInterceptReturnNullAndResetReturnsPointer),
      CU_TEST_INFO(ReallocInterceptCountIncrementsAndResets),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Malloc Wrapper Tests",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
