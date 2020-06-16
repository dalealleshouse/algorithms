// Copyright 2020 Dale Alleshouse
#include "array.h"

#include <stdlib.h>
#include <string.h>

ResultCode Array_Create(comparator comparator, size_t item_size,
                        Array** result) {
  Array* array = calloc(sizeof(Array), 1);

  if (array == NULL) return kFailedMemoryAllocation;

  array->item_size = item_size;
  array->comparator = comparator;

  *result = array;
  return kSuccess;
}

ResultCode Array_Insert(Array* self, const void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  if (self->n == 0) {
    self->array = malloc(self->item_size);
    if (self->array == NULL) return kFailedMemoryAllocation;

    memcpy(self->array, item, self->item_size);
    self->n++;
  } else {
    void* arr = realloc(self->array, (self->n + 1) * self->item_size);
    if (arr == NULL) return kFailedMemoryAllocation;

    self->array = arr;
    memmove((char*)self->array + self->item_size, self->array,
            self->item_size * self->n);
    self->n++;
    memcpy(self->array, item, self->item_size);
  }

  return kSuccess;
}

ResultCode Array_Search(const Array* self, const void* item, void** result) {
  if (self == NULL || item == NULL) return kNullParameter;

  for (size_t i = 0; i < self->n; i++) {
    size_t offset = i * self->item_size;
    int cmp_result = self->comparator(item, (char*)self->array + offset);
    if (cmp_result == 0) {
      *result = (char*)self->array + offset;
      return kSuccess;
    }
  }

  return kNotFound;
}

ResultCode Array_Enumerate(const Array* self, item_handler item_handler) {
  if (self == NULL || item_handler == NULL) return kNullParameter;

  for (size_t pos = 0; pos < self->n; pos++) {
    item_handler((char*)self->array + (self->item_size * pos));
  }

  return kSuccess;
}

void Array_Destroy(Array* self) {
  if (self == NULL) return;

  free(self->array);
  free(self);
}
