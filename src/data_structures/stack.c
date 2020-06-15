#include "stack.h"

#include <stdlib.h>

Stack* Stack_Create() {
  Stack* stack = calloc(sizeof(Stack), 1);
  if (stack == NULL) return NULL;

  return stack;
}

StackResult Stack_Push(Stack* self, void* item) {
  if (self == NULL || item == NULL) return Stack_kNullParameter;

  Stack_Item* s_item = calloc(sizeof(Stack_Item), 1);

  if (s_item == NULL) return Stack_FailedMalloc;

  s_item->payload = item;
  if (self->n == 0) {
    self->head = s_item;
  } else {
    s_item->next = self->head;
    self->head = s_item;
  }

  self->n++;

  return Stack_kSuccess;
}

void* Stack_Pop(Stack* self) {
  if (self == NULL || self->n == 0) return NULL;

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
  return item;
}

bool Stack_IskEmpty(Stack* self) {
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
