/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "CUnit/CUnit.h"
#include "comparators.h"
#include "test_helpers.h"

extern const size_t n;
extern const size_t test_data_n;

typedef struct TestStruct {
  int foo;
  int bar;
  int sorter;
} TestStruct;

int StructComparator(const void* x, const void* y);
int PointerComparator(const void* x, const void* y);

void* Duplicate(const void* ptr, const size_t size);

void TestArraysAreEqual(const size_t n, const size_t size, const void* expected,
                        const void* actual);

void* SortWithC(const size_t n, const size_t size, const void* arr,
                const sort_strategy comparator);

typedef void* (*ArrayGenerator)(const size_t n);
void* SequencedArrayGenerator(size_t n);
void* ReversedArrayGenerator(size_t n);
void* RandomArrayGenerator(size_t n);
void* RandomStructArrayGenerator(size_t n);
void* RandomPointerArrayGenerator(size_t n);
void* DuplicateValueArrayGenerator(size_t n);

void FreePointerArray(size_t n, void** arr);

int* GenerateTestData();

#define STANDARD_SORTING_TESTS(sorter)                                         \
  static void* TestArray(const size_t n, const size_t size,                    \
                         const ArrayGenerator array_generator,                 \
                         const sort_strategy comparator) {                     \
    void* arr = array_generator(n);                                            \
    void* expected = SortWithC(n, size, arr, comparator);                      \
    int result = sorter(n, size, arr, comparator);                             \
                                                                               \
    CU_ASSERT_EQUAL(0, result);                                                \
    TestArraysAreEqual(n, size, expected, arr);                                \
    free(expected);                                                            \
    return arr;                                                                \
  }                                                                            \
                                                                               \
  static void TestIntArray(const size_t n,                                     \
                           const ArrayGenerator array_generator) {             \
    free(TestArray(n, sizeof(int), array_generator, PIntComparator));          \
  }                                                                            \
                                                                               \
  static void sorter##_null_parameter() {                                      \
    ResultCode result_code = sorter(0, 0, NULL, NULL);                         \
    CU_ASSERT_EQUAL(result_code, kNullParameter);                              \
  }                                                                            \
                                                                               \
  static void sorter##_invalid_parmeter() {                                    \
    void* arr = RandomArrayGenerator(n);                                       \
    ResultCode result_code = sorter(0, 0, arr, PIntComparator);                \
    CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);                         \
    free(arr);                                                                 \
  }                                                                            \
                                                                               \
  static void sorter##_single_item_array() {                                   \
    TestIntArray(1, RandomArrayGenerator);                                     \
  }                                                                            \
                                                                               \
  static void sorter##_small_array() {                                         \
    TestIntArray(6, RandomArrayGenerator);                                     \
  }                                                                            \
                                                                               \
  static void sorter##_odd_array() { TestIntArray(7, RandomArrayGenerator); }  \
                                                                               \
  static void sorter##_standard_case() {                                       \
    TestIntArray(n, RandomArrayGenerator);                                     \
  }                                                                            \
                                                                               \
  static void sorter##_pre_sorted() {                                          \
    TestIntArray(n, SequencedArrayGenerator);                                  \
  }                                                                            \
                                                                               \
  static void sorter##_reversed() { TestIntArray(n, ReversedArrayGenerator); } \
                                                                               \
  static void sorter##_dup_items() {                                           \
    TestIntArray(n, DuplicateValueArrayGenerator);                             \
  }                                                                            \
                                                                               \
  static void sorter##_struct_array() {                                        \
    free(TestArray(n, sizeof(TestStruct), RandomStructArrayGenerator,          \
                   *StructComparator));                                        \
  }                                                                            \
                                                                               \
  static void sorter##_ptr_array() {                                           \
    void* arr = TestArray(n, sizeof(void*), RandomPointerArrayGenerator,       \
                          PointerComparator);                                  \
    FreePointerArray(n, arr);                                                  \
  }                                                                            \
                                                                               \
  int Register##sorter##Tests() {                                              \
    CU_TestInfo hash_table_tests[] = {                                         \
        CU_TEST_INFO(sorter##_null_parameter),                                 \
        CU_TEST_INFO(sorter##_invalid_parmeter),                               \
        CU_TEST_INFO(sorter##_single_item_array),                              \
        CU_TEST_INFO(sorter##_small_array),                                    \
        CU_TEST_INFO(sorter##_standard_case),                                  \
        CU_TEST_INFO(sorter##_odd_array),                                      \
        CU_TEST_INFO(sorter##_pre_sorted),                                     \
        CU_TEST_INFO(sorter##_reversed),                                       \
        CU_TEST_INFO(sorter##_struct_array),                                   \
        CU_TEST_INFO(sorter##_ptr_array),                                      \
        CU_TEST_INFO(sorter##_dup_items),                                      \
        CU_TEST_INFO_NULL};                                                    \
                                                                               \
    CU_SuiteInfo suites[] = {{.pName = #sorter,                                \
                              .pInitFunc = noop,                               \
                              .pCleanupFunc = noop,                            \
                              .pTests = hash_table_tests},                     \
                             CU_SUITE_INFO_NULL};                              \
                                                                               \
    return CU_register_suites(suites);                                         \
  }
