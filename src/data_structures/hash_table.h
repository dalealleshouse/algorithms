/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "comparators.h"
#include "result_code.h"

typedef struct KeyValuePair {
  void* key;
  size_t key_length;
  void* value;
} KeyValuePair;

typedef struct HashTable HashTable;
typedef void (*keyvaluehandler)(const KeyValuePair* x, const size_t index,
                                void* context);

ResultCode HashTable_Create(size_t, HashTable**);
ResultCode HashTable_Put(HashTable*, void* key, size_t length, void* value);
ResultCode HashTable_Remove(HashTable*, void* key, size_t length);
ResultCode HashTable_Enumerate(HashTable*, keyvaluehandler, void* context);
ResultCode HashTable_Get(HashTable*, void* key, size_t length, void** result);
bool HashTable_KeyExists(HashTable*, void* key, size_t length);
double HashTable_GetLoadFactor(HashTable*);
size_t HashTable_GetN(HashTable*);
size_t HashTable_GetCollisions(HashTable*);
void HashTable_Destroy(HashTable*, freer);
