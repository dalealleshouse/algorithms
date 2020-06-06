#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "../utils/common.h"
#include "../utils/error_reporter.h"

#define PQ_ERROR(result)                                                \
  {                                                                     \
    char str[1000];                                                     \
    sprintf(str, "PQ Error: %s, %s, %s, %d\n", PQ_ErrorMessage(result), \
            __FILE__, __FUNCTION__, __LINE__);                          \
                                                                        \
    ErrorReporter_Report(result, str);                                  \
  }

typedef enum {
  PQ_ItemNotFound = -4,
  PQ_EmptyQueue = -3,
  PQ_NullParameter = -2,
  PQ_FailedMalloc = -1,
  PQ_Success = 0
} PQResult;

typedef struct PQ_Item {
  void* payload;
  struct PQ_Item* next;
  struct PQ_Item* prev;
} PQ_Item;

typedef struct PriorityQueue {
  size_t n;
  comparator comparator;
  PQ_Item* head;
  PQ_Item* tail;
} PriorityQueue;

PriorityQueue* PQ_Create(comparator);
PQResult PQ_Insert(PriorityQueue*, void*);
PQResult PQ_Reprioritize(PriorityQueue*, void*);
void* PQ_Peek(PriorityQueue*);
void* PQ_Remove(PriorityQueue*);
bool PQ_IsEmpty(PriorityQueue*);
void PQ_Destroy(PriorityQueue*, freer);
char* PQ_ErrorMessage(PQResult);
