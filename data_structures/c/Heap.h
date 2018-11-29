#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "include/CommonTypes.h"
#include "include/ErrorReporter.h"

#define HEAP_ERROR(result)                                                     \
    {                                                                          \
        char str[1000];                                                        \
        sprintf(str, "Heap Error: %s, %s, %s, %d\n",                           \
            Heap_ErrorMessage(result), __FILE__, __FUNCTION__, __LINE__);      \
                                                                               \
        ErrorReporter_Report(result, str);                                     \
    }

typedef enum {
    HeapEmpty = -4,
    HeapFailedMemoryAllocation = -3,
    HeapInvalidSize = -2,
    HeapNullParameter = -1,
    HeapSuccess = 0
} HeapResult;

typedef struct Heap {
    size_t n;
    size_t size;
    comparator comparator;
    void* data[];
} Heap;

Heap* Heap_Create(size_t, comparator);
HeapResult Heap_Insert(Heap*, void*);
void* Heap_Extract(Heap*);
bool Heap_IsEmpty(Heap*);
void* Heap_Find(Heap*);
void Heap_Destroy(Heap*, freer);
char* Heap_ErrorMessage(HeapResult);
