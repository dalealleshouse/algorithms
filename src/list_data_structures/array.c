/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "array.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static ResultCode Array_CreateFirstItem(Array* self, const void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  self->array = malloc(self->item_size);
  if (self->array == NULL) return kFailedMemoryAllocation;

  memcpy(self->array, item, self->item_size);
  self->n++;

  return kSuccess;
}

ResultCode Array_Create(comparator comparator, size_t item_size,
                        Array** result) {
  if (comparator == NULL || result == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;

  Array* array = calloc(sizeof(Array), 1);

  if (array == NULL) return kFailedMemoryAllocation;

  array->item_size = item_size;
  array->comparator = comparator;

  *result = array;
  return kSuccess;
}

ResultCode Array_InsertAtHead(Array* self, const void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  if (self->n == 0) {
    ResultCode result_code = Array_CreateFirstItem(self, item);
    if (result_code != kSuccess) return result_code;
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

ResultCode Array_InsertAtTail(Array* self, const void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  if (self->n == 0) {
    ResultCode result_code = Array_CreateFirstItem(self, item);
    if (result_code != kSuccess) return result_code;
  } else {
    void* arr = realloc(self->array, (self->n + 1) * self->item_size);
    if (arr == NULL) return kFailedMemoryAllocation;

    self->array = arr;
    memcpy((char*)self->array + (self->item_size * self->n), item,
           self->item_size);
    self->n++;
  }

  return kSuccess;
}

ResultCode Array_Search(const Array* self, const void* item, void** result) {
  if (self == NULL || item == NULL || result == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;

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

ResultCode Array_Max(const Array* self, void** result) {
  if (self == NULL || result == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (self->n == 0) return kEmpty;

  void* max = self->array;

  for (size_t i = 1; i < self->n; i++) {
    size_t offset = i * self->item_size;
    int cmp_result = self->comparator(max, (char*)self->array + offset);
    if (cmp_result == -1) {
      max = (char*)self->array + offset;
    }
  }

  *result = max;
  return kSuccess;
}

ResultCode Array_Predecessor(const Array* self, const void* search_for,
                             void** result) {
  if (self == NULL || search_for == NULL || result == NULL) {
    return kNullParameter;
  }
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (self->n == 0) return kEmpty;

  void* pred_canidate = NULL;
  bool found = false;

  for (size_t i = 0; i < self->n; i++) {
    size_t offset = i * self->item_size;

    int cmp_result = self->comparator(search_for, (char*)self->array + offset);

    // Determine if search_for exists in the array
    if (cmp_result == 0) {
      found = true;
    } else {
      // Compare current item to current predecessor candidate
      int pred_cmp_result = 1;
      if (pred_canidate != NULL) {
        pred_cmp_result =
            self->comparator((char*)self->array + offset, pred_canidate);
      }

      // To become the current predecessor candidate:
      //  1) current item must be less than search_for
      //  2) current item must be greater than current candidate (or current
      //  candidate is null)
      if (cmp_result > 0 && pred_cmp_result > 0) {
        pred_canidate = (char*)self->array + offset;
      }
    }
  }

  if (!found) return kNotFound;
  if (pred_canidate == NULL) return kArgumentOutOfRange;

  *result = pred_canidate;
  return kSuccess;
}

ResultCode Array_Rank(const Array* self, const void* item, size_t* result) {
  if (self == NULL || item == NULL || result == NULL) return kNullParameter;
  if (self->n == 0) return kEmpty;

  size_t rank = 0;
  bool found = false;

  for (size_t i = 0; i < self->n; i++) {
    size_t offset = i * self->item_size;

    int cmp_result = self->comparator((char*)self->array + offset, item);

    // increment rank by 1 for every item less than item
    if (cmp_result == 0) {
      found = true;
    } else if (cmp_result < 0) {
      rank++;
    }
  }

  if (!found) return kNotFound;

  *result = rank;
  return kSuccess;
}

void Array_Destroy(Array* self) {
  if (self == NULL) return;

  free(self->array);
  free(self);
}
