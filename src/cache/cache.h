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

typedef void* (*producer)(const void* key, const size_t key_size);
typedef struct Cache Cache;

/*
 * Allocates and initializes a Cache data structure
 *
 *  <limit> = max number of item to store in the cache before purging items
 *  <freer> = function used to free objects created by the producer (see
 *  Cache_Get)
 */
ResultCode Cache_Create(size_t limit, freer freer, Cache**);

/*
 * If key exists it cache, it will return it. If it does not, it will store the
 * item returned from producer in the cache. When the number of items in the
 * cache exceeds <limit>, it will purge the least recently used item
 *
 *  <key> = key to associate with the items
 *  <key_size> = number of bytes in the key value
 *  <producer> = should return the item to store in the cache if it exists
 *  <result> = output parameter for result
 */
ResultCode Cache_Get(Cache*, void* key, size_t key_size, producer,
                     void** result);
void Cache_Destroy(Cache*);
