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

#include "hash_functions.h"

static const size_t byte_size = 8;
static const char* module_name = "BloomFilter";

typedef struct BloomFilter {
  size_t n;
  size_t bits;
  size_t funcs;
  char* filter;
} BloomFilter;

static bool is_power_2(size_t n) { return (ceil(log2(n)) == floor(log2(n))); }

ResultCode BloomFilter_Create(size_t bits, size_t funcs, BloomFilter** self) {
  if (bits < byte_size || !is_power_2(bits) || funcs == 0 ||
      funcs > hasher_count) {
    return kArgumentOutOfRange;
  }

  BloomFilter* filter = malloc(sizeof(BloomFilter));
  if (filter == NULL) return kFailedMemoryAllocation;

  filter->filter = calloc(1, bits / byte_size);
  if (filter->filter == NULL) {
    BloomFilter_Destroy(filter);
    return kFailedMemoryAllocation;
  }

  filter->n = 0;
  filter->bits = bits;
  filter->funcs = funcs;

  *self = filter;
  return kSuccess;
}

size_t BloomFilter_GetN(const BloomFilter* self) {
  if (self == NULL) return 0;

  return self->n;
}

ResultCode BloomFilter_Insert(BloomFilter* self, const char* key) {
  if (self == NULL || key == NULL) return kNullParameter;

  size_t len = strlen(key);

  for (size_t i = 0; i < self->funcs; i++) {
    hash hash = hashers[i]((void*)key, len);
    size_t index = mul_compressor64(hash, self->bits);
    size_t byte_index = index / byte_size;
    size_t bit_index = index % byte_size;

    self->filter[byte_index] =
        (char)(self->filter[byte_index] | 1 << bit_index);
  }

  self->n++;
  return kSuccess;
}

ResultCode BloomFilter_Lookup(const BloomFilter* self, const char* key,
                              bool* result) {
  if (self == NULL || key == NULL) return kNullParameter;

  size_t len = strlen(key);

  for (size_t i = 0; i < self->funcs; i++) {
    hash hash = hashers[i]((void*)key, len);
    size_t index = mul_compressor64(hash, self->bits);
    size_t byte_index = index / byte_size;
    size_t bit_index = index % byte_size;

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
