#ifndef __SORTING_TEST_HELPERS_TEST_H
#define __SORTING_TEST_HELPERS_TEST_H

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

#endif
