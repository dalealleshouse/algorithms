/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/

#include "bit_vector.h"

#include <stdlib.h>

#include "common_math.h"

typedef unsigned char Byte;
static const size_t kByteSize = 8;

typedef struct BitVector {
  size_t bits;
  Byte data[];
} BitVector;

ResultCode BitVector_Create(size_t size, BitVector** self) {
  if (size == 0) return kArgumentOutOfRange;
  if (self == NULL) return kNullParameter;
  if (*self != NULL) return kOutputPointerIsNotNull;

  size_t actual_bits = NextPowerOfTwo(size);
  BitVector* bv = calloc(1, sizeof(BitVector) + (actual_bits / kByteSize));
  if (bv == NULL) return kFailedMemoryAllocation;

  bv->bits = actual_bits;
  *self = bv;
  return kSuccess;
}

ResultCode BitVector_Set(BitVector* self, size_t index, BitValue value) {
  if (self == NULL) return kNullParameter;
  if (index >= self->bits) return kArgumentOutOfRange;
  if (value != kSet && value != kUnset) return kArgumentOutOfRange;

  size_t byte_index = index / kByteSize;
  size_t bit_index = index % kByteSize;
  Byte mask = 1 << bit_index;

  self->data[byte_index] =
      (self->data[byte_index] & ~mask) | ((value << bit_index) & mask);

  return kSuccess;
}

ResultCode BitVector_Read(BitVector* self, size_t index, BitValue* result) {
  if (self == NULL) return kNullParameter;
  if (index >= self->bits) return kArgumentOutOfRange;

  size_t byte_index = index / kByteSize;
  size_t bit_index = index % kByteSize;
  Byte mask = 1 << bit_index;
  Byte value = self->data[byte_index] & mask;

  *result = (value > 0) ? kSet : kUnset;

  return kSuccess;
}

void BitVector_Destroy(BitVector* self) {
  if (self == NULL) return;

  free(self);
}
