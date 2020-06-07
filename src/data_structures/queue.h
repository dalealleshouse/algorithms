#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum {
  Queue_NullParameter = -2,
  Queue_FailedMalloc = -1,
  Queue_Success = 0
} QueueResult;

typedef struct Queue_Item {
  void* payload;
  struct Queue_Item* next;
} Queue_Item;

typedef struct Queue {
  size_t n;
  Queue_Item* head;
  Queue_Item* tail;
} Queue;

Queue* Queue_Create();
QueueResult Queue_Enqueue(Queue*, void*);
void* Queue_Dequeue(Queue*);
bool Queue_IsEmpty(Queue*);
void Queue_Destroy(Queue*);
