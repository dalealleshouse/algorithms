#pragma once

#include <stdbool.h>

#include "../utils/result_code.h"

typedef struct BloomFilter BloomFilter;
const size_t N_ERROR;

BloomFilter* BloomFilter_Create(size_t bits, size_t funcs);
size_t BloomFilter_GetN(const BloomFilter*);
ResultCode BloomFilter_Insert(BloomFilter*, const char* key);
bool BloomFilter_Lookup(const BloomFilter*, const char* key);
void BloomFilter_Destroy(BloomFilter*);
