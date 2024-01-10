/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "bloom_filter.h"

#include <stdlib.h>
#include <string.h>

#include "bit_vector.h"
#include "hash_functions.h"

typedef struct BloomFilter {
  size_t n;
  size_t bits;
  BitVector* filter;
  size_t hasher_count;
  const hasher* hashers;
} BloomFilter;

ResultCode BloomFilter_Create(size_t bits, const hasher hashers[],
                              const size_t hasher_count, BloomFilter** self) {
  if (hasher_count == 0) return kArgumentOutOfRange;

  BloomFilter* filter = malloc(sizeof(BloomFilter));
  if (filter == NULL) return kFailedMemoryAllocation;

  filter->filter = NULL;
  ResultCode result_code = BitVector_Create(bits, &filter->filter);
  if (result_code != kSuccess) {
    BloomFilter_Destroy(filter);
    return result_code;
  }

  filter->n = 0;
  filter->bits = BitVector_GetBitCount(filter->filter);
  filter->hasher_count = hasher_count;
  filter->hashers = hashers;

  *self = filter;
  return kSuccess;
}

size_t BloomFilter_GetN(const BloomFilter* self) {
  if (self == NULL) return 0;

  return self->n;
}

ResultCode BloomFilter_Insert(BloomFilter* self, const void* key,
                              size_t key_size) {
  if (self == NULL || key == NULL) return kNullParameter;

  for (size_t i = 0; i < self->hasher_count; i++) {
    hash hash = hashers[i]((void*)key, key_size);
    size_t index = mul_compressor64(hash, self->bits);

    ResultCode result_code = BitVector_Set(self->filter, index, kSet);
    if (result_code != kSuccess) return result_code;
  }

  self->n++;
  return kSuccess;
}

ResultCode BloomFilter_Lookup(const BloomFilter* self, const void* key,
                              size_t key_size, BloomFilterResult* result) {
  if (self == NULL || key == NULL) return kNullParameter;

  for (size_t i = 0; i < self->hasher_count; i++) {
    hash hash = hashers[i]((void*)key, key_size);
    size_t index = mul_compressor64(hash, self->bits);

    BitValue bit_value;
    ResultCode result_code = BitVector_Read(self->filter, index, &bit_value);
    if (result_code != kSuccess) return result_code;

    if (bit_value == kUnset) {
      *result = kDefiniteNo;
      return kSuccess;
    }
  }

  *result = kHighlyProbable;
  return kSuccess;
}

void BloomFilter_Destroy(BloomFilter* self) {
  if (self == NULL) return;

  BitVector_Destroy(self->filter);
  free(self);
}
