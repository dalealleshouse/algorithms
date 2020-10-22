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

ResultCode Cache_Create(size_t limit, freer freer, Cache**);
ResultCode Cache_Get(Cache*, void* key, size_t key_size, producer,
                     void** result);
void Cache_Destroy(Cache*);
