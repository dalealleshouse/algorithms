/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>

#include "comparators.h"
#include "result_code.h"

typedef void (*item_handler)(void* x);
typedef struct {
  comparator comparator;
  size_t n;
  size_t item_size;
  void* array;
} Array;

ResultCode Array_Create(comparator, size_t item_size, Array**);
ResultCode Array_Insert(Array*, const void*);
ResultCode Array_Search(const Array*, const void*, void**);
ResultCode Array_Enumerate(const Array*, item_handler);
void Array_Destroy(Array*);
