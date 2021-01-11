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

int IntComparator(const void* x, const void* y) {
  if (x == y) return 0;

  int _x = *(int*)x;
  int _y = *(int*)y;
  return _x - _y;
}

int StructComparator(const void* x, const void* y) {
  if (x == y) return 0;

  if (x == NULL && y != NULL) return 0;

  if (y == NULL && x != NULL) return 1;

  TestStruct* _x = (TestStruct*)x;
  TestStruct* _y = (TestStruct*)y;

  return IntComparator(&_x->sorter, &_y->sorter);
}

int PointerComparator(const void* x, const void* y) {
  if (x == y || (x == NULL && y == NULL)) return 0;

  if (x == NULL && y != NULL) return 0;

  if (y == NULL && x != NULL) return 1;

  TestStruct* _x = *(TestStruct**)x;
  TestStruct* _y = *(TestStruct**)y;

  return IntComparator(&_x->sorter, &_y->sorter);
}

void* Duplicate(const void* ptr, const size_t size) {
  void* new_ptr = malloc(size);
  memcpy(new_ptr, ptr, size);
  return new_ptr;
}

void TestArraysAreEqual(const size_t n, const size_t size, const void* expected,
                        const void* actual) {
  CU_ASSERT_EQUAL(memcmp(expected, actual, n * size), 0);
}

void* SortWithC(const size_t n, const size_t size, const void* arr,
                const sort_strategy comparator) {
  void* new = Duplicate(arr, n * size);
  qsort(new, n, size, comparator);
  return new;
}

void* SequencedArrayGenerator(size_t n) {
  int* arr = malloc(sizeof(int) * n);
  for (size_t i = 0; i < n; i++) arr[i] = i;

  return arr;
}

void* ReversedArrayGenerator(size_t n) {
  int* arr = malloc(sizeof(int) * n);
  for (size_t i = 0; i < n; i++) arr[i] = (int)n - (int)i;

  return arr;
}

void* RandomArrayGenerator(size_t n) {
  int* arr = malloc(sizeof(int) * n);
  for (size_t i = 0; i < n; i++) arr[i] = (int)(random() % INT_MAX);

  return arr;
}

void* RandomStructArrayGenerator(size_t n) {
  TestStruct* arr = malloc(sizeof(TestStruct) * n);

  for (size_t i = 0; i < n; i++) {
    arr[i] = (TestStruct){i, (int)i + 1, (int)(random() % INT_MAX)};
  }

  return arr;
}

void* RandomPointerArrayGenerator(size_t n) {
  TestStruct** arr = malloc(sizeof(void*) * n);

  for (size_t i = 0; i < n; i++) {
    TestStruct* tmp = malloc(sizeof(TestStruct));
    *tmp = (TestStruct){i, (int)i + 1, (int)(random() % INT_MAX)};

    arr[i] = tmp;
  }

  return arr;
}

void* DuplicateValueArrayGenerator(size_t n) {
  int* arr = malloc(sizeof(int) * n);
  for (size_t i = 0; i < n; i++) arr[i] = (int)(random() % 10);

  return arr;
}

void FreePointerArray(size_t n, void** arr) {
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
