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

/*
 * Partitions the array around the first item. All items less than the first
 * item will be before it and all items greater that the first item will be
 * after it.
 *
 * Sets the value of <pivot_index> to the index of the first item after
 * partitioning around it
 */
ResultCode Partition(const size_t n, const size_t size, void* arr,
                     const comparator comparator, size_t* pivot_index);

typedef int (*choose_pivot)(const size_t n, const size_t size, const void* arr,
                            const comparator comparator);

int PivotOnZero(const size_t n, const size_t size, const void* arr,
                const comparator comparator);
int PivotOnRandom(const size_t n, const size_t size, const void* arr,
                  const comparator comparator);
int PivotOnLast(const size_t n, const size_t size, const void* arr,
                const comparator comparator);
int PivotOnMedian(const size_t n, const size_t size, const void* arr,
                  const comparator comparator);

ResultCode QuickSortPivot(const size_t n, const size_t size, void* arr,
                          const comparator comparator,
                          choose_pivot choose_pivot);

ResultCode QuickSort(const size_t n, const size_t size, void* arr,
                     const comparator comparator);
