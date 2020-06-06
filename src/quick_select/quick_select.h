#pragma once

#include <stddef.h>
#include "../utils/common.h"

typedef int (*choose_pivot)(const size_t n);

/*
 * Partitions the array around the first item. All items less than the first
 * item will be before it and all items greater that the first item will be
 * after it.
 *
 * returns: On success, the index of the first item after partitioning around
 * it. A value less than 0 on failure.
 */
int select_partition(const size_t n, const size_t size, void* arr,
                     const comparator comparator);

typedef int (*choose_pivot)(const size_t n);

int select_pivot_on_zero(const size_t n);
int select_pivot_on_random(const size_t n);

void* quick_select_pivot(const size_t nth, const size_t n, const size_t size,
                         void* arr, const comparator comparator,
                         const choose_pivot choose_pivot);

/*
 * Finds the <nth> ranked value in the array without sorting the entire array.
 *
 * WARNING: the order of the items in <arr> will be modified
 *
 * returns: pointer to the value that represents the <nth> ranked value in <arr>
 */
void* quick_select(const size_t nth, const size_t n, const size_t size,
                   void* arr, const comparator comparator);

/*
 * Same function as quick_select, but instead of partitioning, it just sorts the
 * array with qsort and then selected the nth item.
 */
void* sort_select(const size_t nth, const size_t n, const size_t size,
                  void* arr, const comparator comparator);
