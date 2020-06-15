#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "../utils/comparators.h"
#include "../utils/result_code.h"

typedef struct KeyValuePair {
  void* key;
  size_t key_length;
  void* value;
} KeyValuePair;

const size_t ERROR_VAL;
typedef struct HashTable HashTable;
typedef void (*keyvaluehandler)(const KeyValuePair* x, const size_t index,
                                void* context);

HashTable* HashTable_Create(size_t);
Result HashTable_Insert(HashTable*, void* key, size_t length, void* value);
Result HashTable_Delete(HashTable*, void* key, size_t length);
Result HashTable_Enumerate(HashTable*, keyvaluehandler, void* context);
bool HashTable_KeyExists(HashTable*, void* key, size_t length);
double HashTable_GetLoadFactor(HashTable*);
size_t HashTable_GetN(HashTable*);
size_t HashTable_GetCollisions(HashTable*);
void* HashTable_Find(HashTable*, void* key, size_t length);
void HashTable_Destroy(HashTable*, freer);
