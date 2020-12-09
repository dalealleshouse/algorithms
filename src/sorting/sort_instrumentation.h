/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <aio.h>

extern size_t copy_count;

// un-comment to enable instrumention
/* #define INSTRUMENT_SORT */

#ifdef INSTRUMENT_SORT

#define INSTRUMENTED_MEMCPY(dest, src, n) \
  {                                       \
    ++copy_count;                         \
    memcpy(dest, src, n);                 \
  }

#define INSTRUMENTED_MEMMOVE(str1, str2, n) \
  {                                         \
    ++copy_count;                           \
    memmove(str1, str2, n);                 \
  }

#else

#define INSTRUMENTED_MEMCPY(dest, src, n) \
  { memcpy(dest, src, n); }

#define INSTRUMENTED_MEMMOVE(str1, str2, n) \
  { memmove(str1, str2, n); }

#endif
