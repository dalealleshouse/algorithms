/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>

#if defined(NDEBUG)
#define FAILED_MALLOC_TEST(code_block) \
  { code_block; }
#else
#define FAILED_MALLOC_TEST(code_block)          \
  {                                             \
    InterceptMalloc();                          \
    code_block;                                 \
    CU_ASSERT_EQUAL(1, MallocInterceptCount()); \
    ResetMalloc();                              \
  }
#endif

void InterceptMalloc(void);
size_t MallocInterceptCount(void);
void ResetMalloc(void);
