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
  size_t bit_count;
  Byte data[];
} BitVector;

typedef struct BitLocator {
  size_t byte_index;
  size_t bit_index;
  Byte mask;
} BitLocator;

static BitLocator LocateBit(size_t index) {
  size_t byte_index = index / kByteSize;
  size_t bit_index = index % kByteSize;
  Byte mask = 1 << bit_index;

  BitLocator result = {byte_index, bit_index, mask};

  return result;
}

ResultCode BitVector_Create(size_t size, BitVector** self) {
  if (size == 0) return kArgumentOutOfRange;
  if (self == NULL) return kNullParameter;
  if (*self != NULL) return kOutputPointerIsNotNull;

  // byte size in powers of two results in compiler optimizations
  size_t actual_bit_count = NextPowerOfTwo(size);
  BitVector* bv = calloc(1, sizeof(BitVector) + (actual_bit_count / kByteSize));
  if (bv == NULL) return kFailedMemoryAllocation;

  bv->bit_count = actual_bit_count;
  *self = bv;
  return kSuccess;
}

ResultCode BitVector_Set(BitVector* self, size_t index, BitValue value) {
  if (self == NULL) return kNullParameter;
  if (index >= self->bit_count) return kArgumentOutOfRange;
  if (value != kSet && value != kUnset) return kArgumentOutOfRange;

  BitLocator bl = LocateBit(index);
  self->data[bl.byte_index] = (self->data[bl.byte_index] & ~bl.mask) |
                              ((value << bl.bit_index) & bl.mask);

  return kSuccess;
}

ResultCode BitVector_Read(BitVector* self, size_t index, BitValue* result) {
  if (self == NULL) return kNullParameter;
  if (index >= self->bit_count) return kArgumentOutOfRange;

  BitLocator bl = LocateBit(index);
  Byte value = self->data[bl.byte_index] & bl.mask;

  *result = (value > 0) ? kSet : kUnset;

  return kSuccess;
}

size_t BitVector_GetBitCount(BitVector* self) {
  if (self == NULL) return 0;

  return self->bit_count;
}

void BitVector_Destroy(BitVector* self) {
  if (self == NULL) return;

  free(self);
}
