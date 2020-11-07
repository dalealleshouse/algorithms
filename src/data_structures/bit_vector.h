/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/

#pragma once

#include <stddef.h>

#include "result_code.h"

typedef struct BitVector BitVector;
typedef enum BitValue { kUnset = 0, kSet = 1 } BitValue;

/*
 * Allocates and initializes a BitVector data structure
 *
 * params:
 *  <size> = number of bits to track
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode BitVector_Create(size_t size, BitVector**);

/*
 * Sets the value of an individual bit
 *
 * params:
 *  <self> = bit vector data structure
 *  <index> = index of the bit to set
 *  <value> = value to set the bit to
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode BitVector_Set(BitVector* self, size_t index, BitValue value);

/*
 * Sets the value of an individual bit
 *
 * params:
 *  <self> = bit vector data structure
 *  <index> = index of the bit to read
 *  <result> = output parameter that will be set to the value of the bit in
 *      question
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode BitVector_Read(BitVector* self, size_t index, BitValue* result);

/*
 * returns the number of individual bits in the bit vector
 */
size_t BitVector_GetBitCount(BitVector* self);

void BitVector_Destroy(BitVector*);
