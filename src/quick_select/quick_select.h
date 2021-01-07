/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
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
 * Find the value that belongs in the <nth> ordinal position
 *
 * params:
 *  <nth>        the ordinal position to identify
 *  <n>          number of items in input array
 *  <size>       size, in bytes, of each array item
 *  <arr>        the array
 *  <comparator> strategy for sorting items
 *  <result>     output parameter that will be set to the address of the <nth>
 *               ordinal item
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode QuickSelect(const size_t nth, const size_t n, const size_t size,
                       void* arr, const sort_strategy comparator,
                       void** result);

/*
 * Same functionality as QuickSelect; however, instead of using the quick sort
 * partitioning algorithm, it simply sorts the array and returns the <nth> item
 *
 * params:
 *  <nth>        the ordinal position to identify
 *  <n>          number of items in input array
 *  <size>       size, in bytes, of each array item
 *  <arr>        the array
 *  <comparator> strategy for sorting items
 *  <result>     output parameter that will be set to the address of the <nth>
 *               ordinal item
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode SortSelect(const size_t nth, const size_t n, const size_t size,
                      void* arr, const sort_strategy comparator, void** result);
