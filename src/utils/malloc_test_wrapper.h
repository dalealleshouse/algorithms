#pragma once

#include <stddef.h>

#define FAILED_MALLOC_TEST(code_block)          \
  {                                             \
    InterceptMalloc();                          \
    code_block;                                 \
    CU_ASSERT_EQUAL(1, MallocInterceptCount()); \
    ResetMalloc();                              \
  }

void InterceptMalloc();
size_t MallocInterceptCount();
void ResetMalloc();
