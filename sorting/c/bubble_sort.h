#ifndef __BUBBLE_SORT_H
#define __BUBBLE_SORT_H

#include <stddef.h>

#include "sorting.h"

int bubble_sort(
    const size_t n, const size_t size, void* arr, const comparator comparator);

#endif
