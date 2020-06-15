#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "../utils/comparators.h"
#include "../utils/error_reporter.h"
#include "./hash_table.h"

#define HEAP_ERROR(result)                                                  \
  {                                                                         \
    char str[1000];                                                         \
    sprintf(str, "Heap Error: %s, %s, %s, %d\n", Heap_ErrorMessage(result), \
            __FILE__, __FUNCTION__, __LINE__);                              \
                                                                            \
    ErrorReporter_Report(result, str);                                      \
  }

typedef enum {
  HeapItemkNotFound = -8,
  HeapHashTableError = -7,
  HeapkArithmeticOverflow = -6,
  HeapOverflow = -5,
  HeapkEmpty = -4,
  HeapkFailedMemoryAllocation = -3,
  HeapInvalidSize = -2,
  HeapkNullParameter = -1,
  HeapkSuccess = 0
} HeapResult;

typedef struct Heap {
  size_t n;
  size_t size;
  comparator comparator;
  void** data;
  HashTable* item_tracker;
} Heap;

Heap* Heap_Create(size_t, comparator);
HeapResult Heap_Insert(Heap*, void*);
HeapResult Heap_Resize(Heap*, size_t);
HeapResult Heap_Reproiritize(Heap*, void*);
void* Heap_Extract(Heap*);
bool Heap_IskEmpty(Heap*);
bool Heap_Exists(Heap*, void*);
void* Heap_Peek(Heap*);
void Heap_Destroy(Heap*, freer);
char* Heap_ErrorMessage(HeapResult);
