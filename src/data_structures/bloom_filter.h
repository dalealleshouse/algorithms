/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stdbool.h>

#include "hash_functions.h"
#include "result_code.h"

typedef struct BloomFilter BloomFilter;
typedef enum BloomFilterResult {
  kDefiniteNo = 0,
  kHighlyProbable = 1
} BloomFilterResult;

/*
 * Allocates and initializes a BloomFilter data structure
 *
 * params:
 *  <bits> = number of bits to track
 *  <hashers> = array of <hashers> functions
 *  <hasher_count> = number of items in the hasher array
 *  <self> = Pointer to a null <BloomFilter>, the function is response for
 *      allocating and initializing it. Passing in a non-null pointer will
 *      result in a <kOutputPointerIsNotNull> result
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode BloomFilter_Create(size_t bits, const hasher hashers[],
                              size_t hasher_count, BloomFilter** self);

/*
 * Adds a key to the bloom filter
 *
 * params:
 *  <self> = bloom filter data structure
 *  <key> = key to add to the filter
 *  <key_size> = size of <key>, in bytes
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode BloomFilter_Insert(BloomFilter* self, const void* key,
                              size_t key_size);

/*
 * Determine if the specified key exists in the filter
 *
 * params:
 *  <self> = bloom filter data structure
 *  <key> = Null terminated string containing a key to lookup
 *  <key_size> = size of <key>, in bytes
 *  <result> = Output parameter that will be set to kHighlyProbable if the item
 *      was found and kDefiniteNo otherwise
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode BloomFilter_Lookup(const BloomFilter*, const void* key,
                              size_t key_size, BloomFilterResult* result);

/*
 * Returns the numbers the of items that have inserted into the bloom filter.
 * WARNING: Multiple inserts of the same item will increment this count.
 *
 * params:
 *  <self> = bloom filter data structure
 *
 * returns:
 *  Number of items inserted into the filter
 */
size_t BloomFilter_GetN(const BloomFilter*);

void BloomFilter_Destroy(BloomFilter*);
