#ifndef __QUICK_SORT_H
#define __QUICK_SORT_H

#include <stddef.h>

#include "test_comparators.h"

int quick_sort(
    const size_t n, const size_t item_size, void* arr, comparator comparator);

#endif
