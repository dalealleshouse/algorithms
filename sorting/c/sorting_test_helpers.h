#pragma once

#include <stddef.h>

#include "sorting.h"

typedef struct test_struct {
    int foo;
    int bar;
    int sorter;
} test_struct_t;

int int_comparator(const void* x, const void* y);
int struct_comparator(const void* x, const void* y);
int pointer_comparator(const void* x, const void* y);

void* dup(const void* ptr, const size_t size);

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

#define STANDARD_SORTING_TESTS(sorter)                                        \
    static void null()                                                        \
    {                                                                         \
        int result = sorter(0, 0, NULL, NULL);                                \
        CU_ASSERT_EQUAL(-1, result);                                          \
    }                                                                         \
                                                                              \
    static void* test_array(const size_t n, const size_t size,                \
        const arr_gen arr_gen, const comparator comparator)                   \
    {                                                                         \
        void* arr = arr_gen(n);                                               \
        void* expected = sort_with_c(n, size, arr, comparator);               \
        int result = sorter(n, size, arr, comparator);                        \
                                                                              \
        CU_ASSERT_EQUAL(0, result);                                           \
        arrays_are_equal(n, size, expected, arr);                             \
                                                                              \
        free(expected);                                                       \
        return arr;                                                           \
    }                                                                         \
                                                                              \
    static void test_int_array(const size_t n, const arr_gen arr_gen)         \
    {                                                                         \
        free(test_array(n, sizeof(int), arr_gen, int_comparator));            \
    }                                                                         \
                                                                              \
    static void single_item_array() { test_int_array(1, rand_arr); }          \
                                                                              \
    static void small_array() { test_int_array(6, rand_arr); }                \
                                                                              \
    static void odd_array() { test_int_array(7, rand_arr); }                  \
                                                                              \
    static void standard_case() { test_int_array(n, rand_arr); }              \
                                                                              \
    static void pre_sorted() { test_int_array(n, seq_arr); }                  \
                                                                              \
    static void reversed() { test_int_array(n, rev_arr); }                    \
                                                                              \
    static void dup_items() { test_int_array(n, dup_val_arr); }               \
                                                                              \
    static void struct_array()                                                \
    {                                                                         \
        free(test_array(                                                      \
            n, sizeof(test_struct_t), rand_st_arr, struct_comparator));       \
    }                                                                         \
                                                                              \
    static void ptr_array()                                                   \
    {                                                                         \
        void* arr                                                             \
            = test_array(n, sizeof(void*), rand_ptr_arr, pointer_comparator); \
        free_ptr_arr(n, arr);                                                 \
    }

#define ADD_STANDARD_TESTS(pSuite)                                            \
    NULL == CU_add_test(pSuite, "null does not throw", null)                  \
        || NULL == CU_add_test(pSuite, "single item", single_item_array)      \
        || NULL == CU_add_test(pSuite, "small", small_array)                  \
        || NULL == CU_add_test(pSuite, "standard case", standard_case)        \
        || NULL == CU_add_test(pSuite, "odd", odd_array)                      \
        || NULL == CU_add_test(pSuite, "pre-sorted", pre_sorted)              \
        || NULL == CU_add_test(pSuite, "reversed", reversed)                  \
        || NULL == CU_add_test(pSuite, "struct", struct_array)                \
        || NULL == CU_add_test(pSuite, "pointers", ptr_array)                 \
        || NULL == CU_add_test(pSuite, "duplicate items", dup_items)
