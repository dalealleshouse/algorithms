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

// There are definitely better ways to do this. However, I need a macro that can
// completely remove the tracking code so it does not have an effect on the
// runtime data. Also, I'm not concerned about concurrency and other common
// problems associated with global variables
#ifdef INSTRUMENT_SORT

#define INSTRUMENTED_MEMCPY(dest, src, n, item_size) \
  {                                                  \
    copy_count += (n) / (item_size);                 \
    memcpy(dest, src, n);                            \
  }

#define INSTRUMENTED_MEMMOVE(str1, str2, n, item_size) \
  {                                                    \
    copy_count += (n) / (item_size);                   \
    memmove(str1, str2, n);                            \
  }

#else

#define INSTRUMENTED_MEMCPY(dest, src, n, item_size) \
  { memcpy(dest, src, n); }

#define INSTRUMENTED_MEMMOVE(str1, str2, n, item_size) \
  { memmove(str1, str2, n); }

#endif
