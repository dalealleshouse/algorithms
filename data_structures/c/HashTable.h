#pragma once

#include <stddef.h>

#include "include/CommonTypes.h"
#include "include/ResultCode.h"

const size_t ERROR_VAL;
typedef struct HashTable HashTable;

HashTable* HashTable_Create(size_t);
Result HashTable_Insert(HashTable*, void* key, size_t length, void* value);
Result HashTable_Delete(HashTable*, void* key, size_t length);
double HashTable_GetLoadFactor(HashTable*);
size_t HashTable_GetN(HashTable*);
size_t HashTable_GetCollisions(HashTable*);
void* HashTable_Find(HashTable*, void* key, size_t length);
void HashTable_Destroy(HashTable*, freer);
