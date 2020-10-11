/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "heap.h"

#include <stdint.h>
#include <stdlib.h>

#include "overflow_checker.h"

struct Heap_t {
  size_t n;
  size_t size;
  comparator comparator;
  void** data;
  HashTable* item_tracker;
};

static size_t ParentIndex(size_t index) {
  ++index;
  index = index >> 1;
  return index - 1;
}

static size_t ChildIndex(size_t index) {
  index++;
  index = index << 1;
  return index - 1;
}

static ResultCode SetDataItem(Heap* self, void* item, size_t index) {
  // Clean up whatever is in the current slot
  void* existing = HashTable_Find(self->item_tracker, &item, sizeof(void*));
  free(existing);

  // Geta a new index
  size_t* i = malloc(sizeof(index));
  if (i == NULL) return kFailedMemoryAllocation;

  // Set the index in the hash table
  *i = index;
  HashTable_Insert(self->item_tracker, &item, sizeof(void*), i);

  // Update the data
  self->data[index] = item;
  return kSuccess;
}

static ResultCode Swap(Heap* self, size_t x, size_t y) {
  void* temp = self->data[x];
  ResultCode result = SetDataItem(self, self->data[y], x);
  if (result != kSuccess) return result;

  result = SetDataItem(self, temp, y);
  if (result != kSuccess) return result;
  /* data[x] = data[y]; */
  /* data[y] = temp; */
  return kSuccess;
}

static ResultCode BubbleUp(Heap* self, size_t start) {
  while (start > 0) {
    size_t parent_index = ParentIndex(start);
    int comp_result =
        self->comparator(self->data[start], self->data[parent_index]);

    if (comp_result <= 0) break;

    ResultCode result = Swap(self, start, parent_index);
    if (result != kSuccess) return result;

    start = parent_index;
  }

  return kSuccess;
}

static size_t GreatestPriority(Heap* self, size_t x, size_t y) {
  if (y >= self->n) return x;
  if (x >= self->n) return y;

  int result = self->comparator(self->data[x], self->data[y]);

  if (result >= 0) return x;

  return y;
}

static ResultCode BubbleDown(Heap* self, size_t start) {
  size_t child;

  while ((child = ChildIndex(start)) < self->n) {
    child = GreatestPriority(self, child, child + 1);

    int comp_result = self->comparator(self->data[start], self->data[child]);

    if (comp_result >= 0) return kSuccess;

    ResultCode result = Swap(self, start, child);
    if (result != kSuccess) return result;

    start = child;
  }

  return kSuccess;
}

ResultCode Heap_Create(size_t size, comparator comparator, Heap** self) {
  if (comparator == NULL || self == NULL) return kNullParameter;
  if (*self != NULL) return kOutputPointerIsNotNull;
  if (size == 0) return kArgumentOutOfRange;
  if (IsMulOverflow_size_t(sizeof(void*), size)) return kArithmeticOverflow;

  Heap* heap = malloc(sizeof(Heap));
  if (heap == NULL) return kFailedMemoryAllocation;
  heap->item_tracker = NULL;

  heap->data = malloc(sizeof(void*) * size);
  if (heap->data == NULL) {
    Heap_Destroy(heap, NULL);
    return kFailedMemoryAllocation;
  }

  heap->item_tracker = HashTable_Create(size);
  if (heap->item_tracker == NULL) {
    Heap_Destroy(heap, NULL);
    return kFailedMemoryAllocation;
  }

  heap->size = size;
  heap->n = 0;
  heap->comparator = comparator;
  *self = heap;

  return kSuccess;
}

void Heap_Destroy(Heap* self, freer freer) {
  if (self == NULL) return;

  HashTable_Destroy(self->item_tracker, free);

  if (freer != NULL && self->data != NULL) {
    for (size_t i = 0; i < self->n; i++) freer(self->data[i]);
  }

  free(self->data);
  free(self);
}

ResultCode Heap_Insert(Heap* self, void* item) {
  if (self == NULL || item == NULL) return kNullParameter;
  if (self->size == self->n) return kOverflow;

  ResultCode result = SetDataItem(self, item, self->n);
  if (result != kSuccess) return result;

  result = BubbleUp(self, self->n);
  if (result != kSuccess) return result;

  self->n++;

  return kSuccess;
}

ResultCode Heap_Resize(Heap* self, size_t size) {
  if (self == NULL) return kNullParameter;
  if (size < self->n) return kArgumentOutOfRange;
  if (IsMulOverflow_size_t(size, sizeof(void*))) return kArithmeticOverflow;

  void* new_data = realloc(self->data, sizeof(void*) * size);
  if (new_data == NULL) return kFailedMemoryAllocation;

  self->data = new_data;
  self->size = size;

  return kSuccess;
}

ResultCode Heap_Extract(Heap* self, void** result) {
  if (self == NULL || result == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (Heap_IsEmpty(self)) return kUnderflow;

  void* item = self->data[0];
  self->n--;
  ResultCode result_code = Swap(self, 0, self->n);
  if (result_code != kSuccess) return result_code;

  result_code = BubbleDown(self, 0);
  if (result_code != kSuccess) return result_code;

  *result = item;
  return kSuccess;
}

ResultCode Heap_Peek(Heap* self, void** result) {
  if (self == NULL || result == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (Heap_IsEmpty(self)) return kUnderflow;

  *result = self->data[0];
  return kSuccess;
}

bool Heap_IsEmpty(Heap* self) {
  if (self == NULL || self->n == 0) return true;
  if (self->n == 0) return true;

  return false;
}

bool Heap_Exists(Heap* self, void* findMe) {
  if (self == NULL || findMe == NULL) return false;
  void* found = HashTable_Find(self->item_tracker, &findMe, sizeof(void*));

  return found != NULL;
}

ResultCode Heap_Reproiritize(Heap* self, void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  size_t* index = HashTable_Find(self->item_tracker, &item, sizeof(void*));
  if (index == NULL) return kNotFound;

  // Index is the first item in queue, so it can only go down
  if (*index == 0) return BubbleDown(self, *index);

  // Figure out if item has a higher priority than it
  size_t parent_index = ParentIndex(*index);
  int comp_result =
      self->comparator(self->data[*index], self->data[parent_index]);

  // If they are equal, no need to move anything
  if (comp_result == 0) return kSuccess;

  // If it is greater, then bubble it up
  if (comp_result > 0) return BubbleUp(self, *index);

  // The only other option is less so bubble it down
  return BubbleDown(self, *index);
}

size_t Heap_Size(Heap* self) {
  if (self == NULL) return 0;

  return self->n;
}

size_t Heap_MaxSize(Heap* self) {
  if (self == NULL) return 0;

  return self->size;
}
