/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "sorting_test_helpers.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "CUnit/CUnit.h"

const size_t n = 200;
const size_t test_data_n = 10000;

void* Malloc(size_t size) {
  void* result = malloc(size);
  if (result == NULL) fprintf(stderr, "malloc failed");

  return result;
}

int int_comparator(const void* x, const void* y) {
  if (x == y) return 0;

  int _x = *(int*)x;
  int _y = *(int*)y;

  if (_x == _y) return 0;

  if (_x < _y) return -1;

  return 1;
}

int StructComparator(const void* x, const void* y) {
  if (x == y) return 0;

  if (x == NULL && y != NULL) return 0;

  if (y == NULL && x != NULL) return 1;

  TestStruct* _x = (TestStruct*)x;
  TestStruct* _y = (TestStruct*)y;

  return int_comparator(&_x->sorter, &_y->sorter);
}

int PointerComparator(const void* x, const void* y) {
  if (x == y || (x == NULL && y == NULL)) return 0;

  if (x == NULL && y != NULL) return 0;

  if (y == NULL && x != NULL) return 1;

  TestStruct* _x = *(TestStruct**)x;
  TestStruct* _y = *(TestStruct**)y;

  return int_comparator(&_x->sorter, &_y->sorter);
}

void* Duplicate(const void* ptr, const size_t size) {
  void* new_ptr = Malloc(size);
  memcpy(new_ptr, ptr, size);
  return new_ptr;
}

void arrays_are_equal(const size_t n, const size_t size, const void* expected,
                      const void* actual) {
  CU_ASSERT_EQUAL(memcmp(expected, actual, n * size), 0);
}

void* sort_with_c(const size_t n, const size_t size, const void* arr,
                  const sort_strategy comparator) {
  void* new = Duplicate(arr, n * size);
  qsort(new, n, size, comparator);
  return new;
}

void* seq_arr(size_t n) {
  int* arr = Malloc(sizeof(int) * n);
  for (size_t i = 0; i < n; i++) arr[i] = i;

  return arr;
}

void* rev_arr(size_t n) {
  int* arr = Malloc(sizeof(int) * n);
  for (size_t i = 0; i < n; i++) arr[i] = (int)n - (int)i;

  return arr;
}

void* rand_arr(size_t n) {
  unsigned int seed = time(NULL);
  int* arr = Malloc(sizeof(int) * n);
  for (size_t i = 0; i < n; i++) arr[i] = rand_r(&seed);

  return arr;
}

void* rand_st_arr(size_t n) {
  unsigned int seed = time(NULL);
  TestStruct* arr = Malloc(sizeof(TestStruct) * n);

  for (size_t i = 0; i < n; i++) {
    arr[i] = (TestStruct){i, (int)i + 1, rand_r(&seed)};
  }

  return arr;
}

void* rand_ptr_arr(size_t n) {
  unsigned int seed = time(NULL);
  TestStruct** arr = Malloc(sizeof(void*) * n);

  for (size_t i = 0; i < n; i++) {
    TestStruct* tmp = Malloc(sizeof(TestStruct));
    *tmp = (TestStruct){i, (int)i + 1, rand_r(&seed)};

    arr[i] = tmp;
  }

  return arr;
}

void* dup_val_arr(size_t n) {
  unsigned int seed = time(NULL);
  int* arr = Malloc(sizeof(int) * n);
  for (size_t i = 0; i < n; i++) arr[i] = rand_r(&seed) % 10;

  return arr;
}

void free_ptr_arr(size_t n, void** arr) {
  for (size_t i = 0; i < n; i++) free(arr[i]);

  free(arr);
}

// No error checking because this is only a test function
int* GenerateTestData() {
  const size_t kBufferSize = 64;

  int* arr = malloc(sizeof(int) * test_data_n);
  FILE* file;

  file = fopen("src/sorting/test_data/sort.txt", "r");

  size_t index = 0;
  int i = 0;

  char line[kBufferSize];
  char* garbage;
  while (fgets(line, kBufferSize, file)) {
    i = (int)strtol(line, &garbage, 10);
    arr[index] = i;
    index++;
  }

  fclose(file);

  return arr;
}
