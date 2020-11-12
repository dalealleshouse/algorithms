/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>

#include "common_math.h"
#include "result_code.h"
#include "test_helpers.h"

const size_t n;

typedef struct TestStruct {
  int foo;
  int bar;
  int sorter;
} TestStruct;

int StructComparator(const void* x, const void* y);
int PointerComparator(const void* x, const void* y);

void* Duplicate(const void* ptr, const size_t size);

void arrays_are_equal(const size_t n, const size_t size, const void* expected,
                      const void* actual);

void* sort_with_c(const size_t n, const size_t size, const void* arr,
                  const comparator comparator);

typedef void* (*arr_gen)(const size_t n);
void* seq_arr(size_t n);
void* rev_arr(size_t n);
void* rand_arr(size_t n);
void* rand_st_arr(size_t n);
void* rand_ptr_arr(size_t n);
void* dup_val_arr(size_t n);

void free_ptr_arr(size_t n, void** arr);

#define STANDARD_SORTING_TESTS(sorter)                                         \
  static void* TestArray(const size_t n, const size_t size,                    \
                         const arr_gen arr_gen, const comparator comparator) { \
    void* arr = arr_gen(n);                                                    \
    void* expected = sort_with_c(n, size, arr, comparator);                    \
    int result = sorter(n, size, arr, comparator);                             \
                                                                               \
    CU_ASSERT_EQUAL(0, result);                                                \
    arrays_are_equal(n, size, expected, arr);                                  \
    free(expected);                                                            \
    return arr;                                                                \
  }                                                                            \
  static void TestIntArray(const size_t n, const arr_gen arr_gen) {            \
    free(TestArray(n, sizeof(int), arr_gen, PIntComparator));                  \
  }                                                                            \
                                                                               \
  static void sorter##_null_parameter() {                                      \
    ResultCode result_code = sorter(0, 0, NULL, NULL);                         \
    CU_ASSERT_EQUAL(result_code, kNullParameter);                              \
  }                                                                            \
                                                                               \
  static void sorter##_invalid_parmeter() {                                    \
    void* arr = rand_arr(n);                                                   \
    ResultCode result_code = sorter(0, 0, arr, PIntComparator);                \
    CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);                         \
    free(arr);                                                                 \
  }                                                                            \
                                                                               \
  static void sorter##_single_item_array() { TestIntArray(1, rand_arr); }      \
                                                                               \
  static void sorter##_small_array() { TestIntArray(6, rand_arr); }            \
                                                                               \
  static void sorter##_odd_array() { TestIntArray(7, rand_arr); }              \
                                                                               \
  static void sorter##_standard_case() { TestIntArray(n, rand_arr); }          \
                                                                               \
  static void sorter##_pre_sorted() { TestIntArray(n, seq_arr); }              \
                                                                               \
  static void sorter##_reversed() { TestIntArray(n, rev_arr); }                \
                                                                               \
  static void sorter##_dup_items() { TestIntArray(n, dup_val_arr); }           \
                                                                               \
  static void sorter##_struct_array() {                                        \
    free(TestArray(n, sizeof(TestStruct), rand_st_arr, *StructComparator));    \
  }                                                                            \
                                                                               \
  static void sorter##_ptr_array() {                                           \
    void* arr = TestArray(n, sizeof(void*), rand_ptr_arr, PointerComparator);  \
    free_ptr_arr(n, arr);                                                      \
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
