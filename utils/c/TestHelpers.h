#pragma once

#include <stddef.h>
#include "MemAllocMock.h"

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

#define UNARY_NULL_TEST(expected_error, func)                                  \
    {                                                                          \
        ErrorReporter_Clear();                                                 \
        void* result = func(NULL);                                             \
        CU_ASSERT_PTR_NULL(result);                                            \
        CU_ASSERT_EQUAL(expected_error, ErrorReporter_LastErrorCode());        \
    }

#define UNARY_NULL_TEST_ENUMERATOR(expected_error, funcs)                      \
    {                                                                          \
        int tracker = 0;                                                       \
        while (funcs[tracker] != NULL) {                                       \
            UNARY_NULL_TEST(expected_error, funcs[tracker]);                   \
            tracker++;                                                         \
        }                                                                      \
    }

#define BINARY_NULL_TEST(expected_error, param1, param2, func)                 \
    {                                                                          \
        ErrorReporter_Clear();                                                 \
        void* result = func(NULL, NULL);                                       \
        CU_ASSERT_PTR_NULL(result);                                            \
        CU_ASSERT_EQUAL(expected_error, ErrorReporter_LastErrorCode());        \
                                                                               \
        ErrorReporter_Clear();                                                 \
        result = func(param1, NULL);                                           \
        CU_ASSERT_PTR_NULL(result);                                            \
        CU_ASSERT_EQUAL(expected_error, ErrorReporter_LastErrorCode());        \
                                                                               \
        ErrorReporter_Clear();                                                 \
        result = func(NULL, param2);                                           \
        CU_ASSERT_PTR_NULL(result);                                            \
        CU_ASSERT_EQUAL(expected_error, ErrorReporter_LastErrorCode());        \
    }

#define BINARY_NULL_TEST_ENUMERATOR(expected_error, param1, param2, funcs)     \
    {                                                                          \
        int tracker = 0;                                                       \
        while (funcs[tracker] != NULL) {                                       \
            BINARY_NULL_TEST(expected_error, param1, param2, funcs[tracker]);  \
            tracker++;                                                         \
        }                                                                      \
    }

#define BINARY_INT_NULL_TEST(expected_error, param1, param2, func)             \
    {                                                                          \
        int result = func(NULL, NULL);                                         \
        CU_ASSERT_EQUAL(expected_error, result);                               \
                                                                               \
        result = func(param1, NULL);                                           \
        CU_ASSERT_EQUAL(expected_error, result);                               \
                                                                               \
        result = func(NULL, param2);                                           \
        CU_ASSERT_EQUAL(expected_error, result);                               \
    }

#define BINARY_INT_NULL_TEST_ENUMERATOR(expected_error, param1, param2, funcs) \
    {                                                                          \
        int tracker = 0;                                                       \
        while (funcs[tracker] != NULL) {                                       \
            BINARY_INT_NULL_TEST(                                              \
                expected_error, param1, param2, funcs[tracker]);               \
            tracker++;                                                         \
        }                                                                      \
    }

#define FAILED_MALLOC_TEST(code_block)                                         \
    {                                                                          \
        MemAllocMock_InterceptMalloc(NULL);                                    \
        code_block;                                                            \
        CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());               \
        MemAllocMock_ResetMalloc();                                            \
    }

typedef struct TestThingy {
    int id;
    size_t n;
    int x;
    int y;
    double z;
} TestThingy;

int noop(void);

void TestFailedMalloc(void (*tester)(void));

int TestRunner(int (*register_tests)());
