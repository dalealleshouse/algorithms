/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/

#include "bit_vector.h"

#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "malloc_test_wrapper.h"
#include "test_helpers.h"

typedef unsigned char Byte;
static const size_t kByteSize = 8;

typedef struct BitVector {
  size_t bits;
  Byte data[];
} BitVector;

#define SUT(size, code_block)                              \
  {                                                        \
    BitVector* sut = NULL;                                 \
    ResultCode result_code = BitVector_Create(size, &sut); \
    CU_ASSERT_EQUAL(result_code, kSuccess);                \
    code_block;                                            \
    BitVector_Destroy(sut);                                \
  }

static void BitVector_Create_invalid_size(void) {
  BitVector* sut = NULL;
  ResultCode result_code = BitVector_Create(0, &sut);
  CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);
  CU_ASSERT_PTR_NULL(sut);
}

static void BitVector_Create_failed_malloc(void) {
#if !defined(NDEBUG)
  FAILED_MALLOC_TEST({
    BitVector* sut = NULL;
    ResultCode result_code = BitVector_Create(10, &sut);
    CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
    CU_ASSERT_PTR_NULL(sut);
  });
#endif
}

static void BitVector_Create_null_parameter(void) {
  ResultCode result_code = BitVector_Create(10, NULL);
  CU_ASSERT_EQUAL(result_code, kNullParameter);

  BitVector* sut = malloc(1);
  result_code = BitVector_Create(10, &sut);
  CU_ASSERT_EQUAL(result_code, kOutputPointerIsNotNull);
  free(sut);
}

static void BitVector_Create_initalizes_bits(void) {
  const size_t bit_count = 128;

  SUT(bit_count, {
    CU_ASSERT_EQUAL(sut->bits, bit_count);

    for (size_t i = 0; i < bit_count / kByteSize; i++) {
      CU_ASSERT_EQUAL(sut->data[i], 0);
    }
  });
}

static void BitVector_Create_smaller_than_byte_size() {
  SUT(6, {
    // It should round up to the next byte increment
    CU_ASSERT_EQUAL(sut->bits, 8);

    CU_ASSERT_EQUAL(sut->data[0], 0);
  });
}

static void BitVector_Set_null_parameter() {
  ResultCode result_code = BitVector_Set(NULL, 1, kUnset);
  CU_ASSERT_EQUAL(result_code, kNullParameter);
}

static void BitVector_Set_index_out_of_range() {
  SUT(16, {
    // valid range is 0-15 for 16 bits
    ResultCode result_code = BitVector_Set(sut, 16, kUnset);
    CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);
  });
}

static void BitVector_Set_value_out_of_range() {
  SUT(16, {
    // valid range is 0-15 for 16 bits
    ResultCode result_code = BitVector_Set(sut, 15, 3);
    CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);
  });
}

static void BitIsSet(Byte byte, size_t bit) {
  Byte mask = 1 << bit;
  Byte result = byte & mask;

  // Verify the bit in question is set
  CU_ASSERT_EQUAL(result, mask);
}

static void AllBitsAreSet(Byte byte) { CU_ASSERT_EQUAL(byte, 255); }

static void BitIsUnset(Byte byte, size_t bit) {
  Byte mask = 1 << bit;
  Byte result = byte & mask;

  // Verify the bit in question is set
  CU_ASSERT_EQUAL(result, 0);
}

static void BitVector_Set_sets_bit_true() {
  const size_t bit_count = 32;

  SUT(bit_count, {
    for (size_t i = 0; i < bit_count; i++) {
      ResultCode result_code = BitVector_Set(sut, i, kSet);
      CU_ASSERT_EQUAL(result_code, kSuccess);

      size_t byte = i / kByteSize;
      size_t bit = i % kByteSize;
      BitIsSet(sut->data[byte], bit);

      // all bits that haven't been set yet should be 0
      for (size_t j = i + 1; j < bit_count; j++) {
        size_t byte = j / kByteSize;
        size_t bit = j % kByteSize;
        BitIsUnset(sut->data[byte], bit);
      }
    }
  });
}

static void BitVector_Set_sets_bit_false() {
  const size_t bit_count = 32;

  SUT(bit_count, {
    for (size_t i = 0; i < bit_count; i++) {
      ResultCode result_code = BitVector_Set(sut, i, kSet);
      CU_ASSERT_EQUAL(result_code, kSuccess);
    }

    // verify all bits are set to 1
    for (size_t i = 0; i < bit_count / kByteSize; i++) {
      AllBitsAreSet(sut->data[i]);
    }

    for (size_t i = 0; i < bit_count; i++) {
      ResultCode result_code = BitVector_Set(sut, i, kUnset);
      CU_ASSERT_EQUAL(result_code, kSuccess);

      size_t byte = i / kByteSize;
      size_t bit = i % kByteSize;
      BitIsUnset(sut->data[byte], bit);
    }
  });
}

static void BitVector_Read_null_parameter() {
  BitValue result;
  ResultCode result_code = BitVector_Read(NULL, 1, &result);
  CU_ASSERT_EQUAL(result_code, kNullParameter);
}

static void BitVector_Read_index_out_of_range() {
  SUT(16, {
    BitValue result;
    // valid range is 0-15 for 16 bits
    ResultCode result_code = BitVector_Read(sut, 16, &result);
    CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);
  });
}

static void BitVector_Read_finds_bit_value() {
  const size_t bit_count = 32;

  SUT(bit_count, {
    for (size_t i = 0; i < bit_count; i++) {
      BitValue result = kSet;
      ResultCode result_code = BitVector_Read(sut, i, &result);
      CU_ASSERT_EQUAL(result_code, kSuccess);
      CU_ASSERT_EQUAL(result, kUnset);

      result_code = BitVector_Set(sut, i, kSet);
      CU_ASSERT_EQUAL(result_code, kSuccess);

      result_code = BitVector_Read(sut, i, &result);
      CU_ASSERT_EQUAL(result_code, kSuccess);
      CU_ASSERT_EQUAL(result, kSet);
    }
  });
}

int RegisterBitVectorTests() {
  CU_TestInfo hash_table_tests[] = {
      CU_TEST_INFO(BitVector_Create_failed_malloc),
      CU_TEST_INFO(BitVector_Create_invalid_size),
      CU_TEST_INFO(BitVector_Create_null_parameter),
      CU_TEST_INFO(BitVector_Create_initalizes_bits),
      CU_TEST_INFO(BitVector_Create_smaller_than_byte_size),
      CU_TEST_INFO(BitVector_Set_null_parameter),
      CU_TEST_INFO(BitVector_Set_index_out_of_range),
      CU_TEST_INFO(BitVector_Set_value_out_of_range),
      CU_TEST_INFO(BitVector_Set_sets_bit_true),
      CU_TEST_INFO(BitVector_Set_sets_bit_false),
      CU_TEST_INFO(BitVector_Read_null_parameter),
      CU_TEST_INFO(BitVector_Read_index_out_of_range),
      CU_TEST_INFO(BitVector_Read_finds_bit_value),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Bit Vector",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = hash_table_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
