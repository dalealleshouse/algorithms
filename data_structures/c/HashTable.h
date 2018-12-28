#pragma once

#include <stddef.h>

#include "include/CommonTypes.h"
#include "include/ResultCode.h"

const size_t ERROR_VAL;
typedef struct HashTable HashTable;

HashTable* HashTable_Create(size_t);
Result HashTable_Insert(HashTable*, char* key, void* value);
Result HashTable_Delete(HashTable*, char* key);
double HashTable_GetLoadFactor(HashTable*);
size_t HashTable_GetN(HashTable*);
void* HashTable_Find(HashTable*, char* key);
void HashTable_Destroy(HashTable*, freer);
