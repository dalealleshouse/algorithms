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
  sort_strategy comparator;
  size_t n;
  size_t item_size;
  void* array;
} Array;

ResultCode Array_Create(sort_strategy, size_t item_size, Array**);
ResultCode Array_InsertAtHead(Array*, const void*);
ResultCode Array_InsertAtTail(Array*, const void*);
ResultCode Array_Search(const Array*, const void*, void**);
ResultCode Array_Enumerate(const Array*, item_handler);
ResultCode Array_Max(const Array*, void**);
ResultCode Array_Predecessor(const Array*, const void*, void**);
ResultCode Array_Rank(const Array*, const void*, size_t*);
void Array_Destroy(Array*);
