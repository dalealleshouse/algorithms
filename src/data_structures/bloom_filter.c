/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "bloom_filter.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "common_math.h"
#include "hash_functions.h"

static const size_t kByteSize = 8;

typedef struct BloomFilter {
  size_t n;
  size_t bits;
  char* filter;
  size_t hasher_count;
  const hasher* hashers;
} BloomFilter;

ResultCode BloomFilter_Create(size_t bits, const hasher hashers[],
                              const size_t hasher_count, BloomFilter** self) {
  if (bits < kByteSize || hasher_count == 0) {
    return kArgumentOutOfRange;
  }

  BloomFilter* filter = malloc(sizeof(BloomFilter));
  if (filter == NULL) return kFailedMemoryAllocation;

  size_t actual_bits = NextPowerOfTwo(bits);
  filter->filter = calloc(1, actual_bits / kByteSize);
  if (filter->filter == NULL) {
    BloomFilter_Destroy(filter);
    return kFailedMemoryAllocation;
  }

  filter->n = 0;
  filter->bits = actual_bits;
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
    size_t byte_index = index / kByteSize;
    size_t bit_index = index % kByteSize;

    self->filter[byte_index] =
        (char)(self->filter[byte_index] | 1 << bit_index);
  }

  self->n++;
  return kSuccess;
}

ResultCode BloomFilter_Lookup(const BloomFilter* self, const void* key,
                              size_t key_size, bool* result) {
  if (self == NULL || key == NULL) return kNullParameter;

  for (size_t i = 0; i < self->hasher_count; i++) {
    hash hash = hashers[i]((void*)key, key_size);
    size_t index = mul_compressor64(hash, self->bits);
    size_t byte_index = index / kByteSize;
    size_t bit_index = index % kByteSize;

    if (!(self->filter[byte_index] & 1 << bit_index)) {
      *result = false;
      return kSuccess;
    }
  }

  *result = true;
  return kSuccess;
}

void BloomFilter_Destroy(BloomFilter* self) {
  free(self->filter);
  free(self);
}
