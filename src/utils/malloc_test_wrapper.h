/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
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
