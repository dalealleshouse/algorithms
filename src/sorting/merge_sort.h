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

// Adapter to expose the same sorting interface as the other sorting algorithms.
ResultCode MergeSortAdapter(const size_t n, const size_t size, void* arr,
                            const sort_strategy comparator);

ResultCode MergeSort(const void* arr, void* output, const size_t length,
                     const size_t item_size, sort_strategy comparator);
