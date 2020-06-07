#pragma once

#include <stddef.h>

#include "./sorting.h"

int merge_sort(const void* arr, void* output, const size_t length,
               const size_t item_size, comparator comparator);
