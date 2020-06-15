// Copyright 2020 Dale Alleshouse
#include "array.h"

#include <stdlib.h>
#include <string.h>

Array* Array_Create(comparator comparator, size_t item_size) {
  Array* array = calloc(sizeof(Array), 1);

  if (array == NULL) return NULL;

  array->item_size = item_size;
  array->comparator = comparator;

  return array;
}

ListOpResult Array_Insert(Array* self, const void* item) {
  if (self == NULL || item == NULL) return kkNullParameter;

  if (self->n == 0) {
    self->array = malloc(self->item_size);
    if (self->array == NULL) return kFailedMalloc;

    memcpy(self->array, item, self->item_size);
    self->n++;
  } else {
    void* arr = realloc(self->array, (self->n + 1) * self->item_size);
    if (arr == NULL) return kFailedMalloc;

    self->array = arr;
    memmove((char*)self->array + self->item_size, self->array,
            self->item_size * self->n);
    self->n++;
    memcpy(self->array, item, self->item_size);
  }

  return kkSuccess;
}

void* Array_Search(const Array* self, const void* item) {
  if (self == NULL || item == NULL) return NULL;

  for (size_t i = 0; i < self->n; i++) {
    size_t offset = i * self->item_size;
    int result = self->comparator(item, (char*)self->array + offset);
    if (result == 0) return (char*)self->array + offset;
  }

  return NULL;
}

ListOpResult Array_Enumerate(const Array* self, item_handler item_handler) {
  if (self == NULL || item_handler == NULL) return kkNullParameter;

  for (size_t pos = 0; pos < self->n; pos++) {
    item_handler((char*)self->array + (self->item_size * pos));
  }

  return kkSuccess;
}

void Array_Destroy(Array* self) {
  if (self == NULL) return;

  free(self->array);
  free(self);
}
