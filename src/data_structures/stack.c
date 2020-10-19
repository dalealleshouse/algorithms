/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "stack.h"

#include <stdlib.h>

typedef struct Stack_Item {
  void* payload;
  struct Stack_Item* next;
} Stack_Item;

struct Stack_t {
  size_t n;
  Stack_Item* head;
};

ResultCode Stack_Create(Stack** self) {
  if (self == NULL) return kNullParameter;
  if (*self != NULL) return kOutputPointerIsNotNull;

  Stack* stack = calloc(sizeof(Stack), 1);
  if (stack == NULL) return kFailedMemoryAllocation;

  *self = stack;
  return kSuccess;
}

ResultCode Stack_Push(Stack* self, void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  Stack_Item* s_item = calloc(sizeof(Stack_Item), 1);
  if (s_item == NULL) return kFailedMemoryAllocation;

  s_item->payload = item;
  if (self->n == 0) {
    self->head = s_item;
  } else {
    s_item->next = self->head;
    self->head = s_item;
  }

  self->n++;

  return kSuccess;
}

ResultCode Stack_Pop(Stack* self, void** result) {
  if (self == NULL || result == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (self->n == 0) return kUnderflow;

  void* item = self->head->payload;
  if (self->n == 1) {
    free(self->head);
    self->head = NULL;
  } else {
    Stack_Item* doomed = self->head;
    self->head = doomed->next;
    free(doomed);
  }

  self->n--;
  *result = item;
  return kSuccess;
}

bool Stack_IsEmpty(Stack* self) {
  if (self == NULL) return true;

  return self->n == 0;
}

void Stack_Destroy(Stack* self) {
  if (self == NULL) return;

  Stack_Item* curr = self->head;
  while (curr != NULL) {
    Stack_Item* temp = curr;
    curr = temp->next;
    free(temp);
  }

  free(self);
}
