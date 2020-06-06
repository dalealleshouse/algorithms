#pragma once

#include <stddef.h>
#include "./sorting.h"

int selection_sort(const size_t n, const size_t size, void* arr,
                   const comparator comparator);
