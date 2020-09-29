/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "queue.h"

#include <stdlib.h>

typedef struct Queue_Item {
  void* payload;
  struct Queue_Item* next;
} Queue_Item;

struct Queue_t {
  size_t n;
  Queue_Item* head;
  Queue_Item* tail;
};

ResultCode Queue_Create(Queue** self) {
  if (self == NULL) return kNullParameter;
  if (*self != NULL) return kOutputPointerIsNotNull;

  Queue* queue = calloc(sizeof(Queue), 1);
  if (queue == NULL) return kFailedMemoryAllocation;

  *self = queue;
  return kSuccess;
}

ResultCode Queue_Enqueue(Queue* self, void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  Queue_Item* q_item = calloc(sizeof(Queue_Item), 1);

  if (q_item == NULL) return kFailedMemoryAllocation;

  q_item->payload = item;
  if (self->n == 0) {
    self->head = q_item;
    self->tail = q_item;
  } else {
    self->tail->next = q_item;
    self->tail = q_item;
  }

  self->n++;

  return kSuccess;
}

ResultCode Queue_Dequeue(Queue* self, void** result) {
  if (self == NULL || result == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (self->n == 0) return kEmpty;

  void* item = self->head->payload;
  if (self->n == 1) {
    free(self->head);
    self->head = NULL;
    self->tail = NULL;
  } else {
    Queue_Item* doomed = self->head;
    self->head = doomed->next;
    free(doomed);
  }

  self->n--;
  *result = item;
  return kSuccess;
}

bool Queue_IsEmpty(Queue* self) {
  if (self == NULL) return true;

  return self->n == 0;
}

void Queue_Destroy(Queue* self) {
  if (self == NULL) return;

  Queue_Item* curr = self->head;
  while (curr != NULL) {
    Queue_Item* temp = curr;
    curr = temp->next;
    free(temp);
  }

  free(self);
}
