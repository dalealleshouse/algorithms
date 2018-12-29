#pragma once

#include <stdbool.h>

#include "include/ResultCode.h"

typedef struct BloomFilter BloomFilter;

BloomFilter* BloomFilter_Create(size_t);
size_t BloomFilter_GetN(const BloomFilter*);
Result BloomFilter_Insert(BloomFilter*, const char* key);
bool BloomFilter_Lookup(BloomFilter*, const char* key);
void BloomFilter_Destroy(BloomFilter*);
