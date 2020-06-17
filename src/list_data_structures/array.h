// Copyright 2020 Hideous Humpback Freak https://hideoushumpbackfreak.com/
#pragma once

#include <stddef.h>

#include "../utils/comparators.h"
#include "../utils/result_code.h"

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
