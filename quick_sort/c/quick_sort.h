#ifndef __QUICK_SORT_H
#define __QUICK_SORT_H

#include <stddef.h>

#include "test_comparators.h"

/*
 * Partitions the array around the first item. All items less than the first
 * item will be before it and all items greater that the first item will be
 * after it.
 *
 * Sets the value of <pivot_index> to the index of the first item after
 * partitioning around it
 *
 * returns: 0 on success, less than 0 on failure
 */
int partition(const size_t n, const size_t size, void* arr,
    const comparator comparator, size_t* pivot_index);

int quick_sort(
    const size_t n, const size_t size, void* arr, const comparator comparator);

#endif
