/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "heap.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

struct Heap_t {
  size_t n;
  size_t size;
  size_t item_size;
  sort_strategy comparator;
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

static bool ItemExists(Heap* self, void* item) {
  return HashTable_KeyExists(self->item_tracker, &item, sizeof(void*));
}

static ResultCode ClearIndex(Heap* self, void* item) {
  size_t* index = NULL;
  ResultCode result_code =
      HashTable_Get(self->item_tracker, &item, sizeof(void*), (void**)&index);
  if (result_code != kSuccess) return result_code;

  free(index);

  return HashTable_Remove(self->item_tracker, &item, sizeof(void*));
}

static ResultCode StoreIndex(Heap* self, void* item, size_t index) {
  if (ItemExists(self, item)) {
    size_t* old = NULL;
    ResultCode result_code =
        HashTable_Get(self->item_tracker, &item, sizeof(void*), (void**)&old);
    if (result_code != kSuccess) return result_code;

    free(old);
  }

  size_t* i = malloc(sizeof(index));
  if (i == NULL) return kFailedMemoryAllocation;

  *i = index;
  return HashTable_Put(self->item_tracker, &item, sizeof(void*), i);
}

static ResultCode FindIndex(Heap* self, void* item, size_t* index) {
  void* stored_index = NULL;
  ResultCode result_code =
      HashTable_Get(self->item_tracker, &item, sizeof(void*), &stored_index);
  if (result_code != kSuccess) return result_code;

  *index = *(size_t*)stored_index;
  return kSuccess;
}

void PrintIndices(Heap* sut) {
  for (size_t i = 0; i < sut->n; i++) {
    void* item = sut->data[i];

    size_t result = 0;
    ResultCode result_code = FindIndex(sut, item, &result);
    if (result_code == kNotFound) {
      printf("%zu = EMPTY\n", i);
    } else {
      printf("%zu = %zu\n", i, result);
    }
  }
}

// Assumption = nothing current exists at index
static ResultCode SetDataItem(Heap* self, void* item, size_t index) {
  // Update the data
  self->data[index] = item;

  // Set the index in the hash table
  return StoreIndex(self, item, index);
}

// Assumption = something exists at x and y
static ResultCode Swap(Heap* self, size_t x, size_t y) {
  size_t newy, newx;

  // Get the index of x stored in the hash table
  ResultCode result_code = FindIndex(self, self->data[x], &newy);
  if (result_code != kSuccess) return result_code;

  // Get the index of y stored in the hash table
  result_code = FindIndex(self, self->data[y], &newx);
  if (result_code != kSuccess) return result_code;

  result_code = StoreIndex(self, self->data[y], newy);
  if (result_code != kSuccess) return result_code;

  result_code = StoreIndex(self, self->data[x], newx);
  if (result_code != kSuccess) return result_code;

  void* temp = self->data[x];
  self->data[x] = self->data[y];
  self->data[y] = temp;

  return kSuccess;
}

static ResultCode MoveItem(Heap* self, void* item, size_t new_index) {
  size_t old_index;

  // Find the location of the item
  ResultCode result_code = FindIndex(self, item, &old_index);
  if (result_code != kSuccess) return result_code;

  // Move the actual data
  self->data[new_index] = self->data[old_index];
  self->data[old_index] = NULL;

  // Update the hash table so it knows where the tail item moved to
  return StoreIndex(self, self->data[new_index], new_index);
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

ResultCode Heap_Create(size_t size, sort_strategy comparator, Heap** self) {
  if (comparator == NULL || self == NULL) return kNullParameter;
  if (*self != NULL) return kOutputPointerIsNotNull;
  if (size == 0) return kArgumentOutOfRange;
  size_t malloc_size;
  if (__builtin_mul_overflow(sizeof(void*), size, &malloc_size)) {
    return kArithmeticOverflow;
  }

  Heap* heap = malloc(sizeof(Heap));
  if (heap == NULL) return kFailedMemoryAllocation;
  heap->item_tracker = NULL;

  // Use calloc so that all data is initalized to NULL
  heap->data = calloc(1, malloc_size);
  if (heap->data == NULL) {
    Heap_Destroy(heap, NULL);
    return kFailedMemoryAllocation;
  }

  ResultCode result_code = HashTable_Create(size, &heap->item_tracker);
  if (result_code != kSuccess) {
    Heap_Destroy(heap, NULL);
    return result_code;
  }

  heap->size = size;
  heap->n = 0;
  heap->comparator = comparator;
  heap->item_size = 8;
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

  // Reject duplicates
  if (ItemExists(self, item)) return kDuplicate;

  ResultCode result = SetDataItem(self, item, self->n);
  if (result != kSuccess) return result;

  self->n++;

  return (self->n == 1) ? kSuccess : BubbleUp(self, self->n - 1);
}

ResultCode Heap_Resize(Heap* self, size_t size) {
  if (self == NULL) return kNullParameter;
  if (size < self->n) return kArgumentOutOfRange;

  size_t malloc_size;
  if (__builtin_mul_overflow(sizeof(void*), size, &malloc_size)) {
    return kArithmeticOverflow;
  }

  void* new_data = realloc(self->data, malloc_size);
  if (new_data == NULL) return kFailedMemoryAllocation;

  self->data = new_data;
  self->size = size;

  return kSuccess;
}

ResultCode Heap_Extract(Heap* self, void** result) {
  if (self == NULL || result == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (Heap_IsEmpty(self)) return kUnderflow;

  // get the return item
  void* item = self->data[0];
  *result = item;
  self->n--;

  // Delete what current in the zero position
  ResultCode result_code = ClearIndex(self, self->data[0]);
  if (result_code != kSuccess) return result_code;

  // If the heap is empty, no need to do anything else
  if (Heap_IsEmpty(self)) goto error;

  // Move the item to the zero position
  result_code = MoveItem(self, self->data[self->n], 0);
  if (result_code != kSuccess) goto error;

  // Bubble the item down into it's rightful place
  return BubbleDown(self, 0);

error:
  return result_code;
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
  return ItemExists(self, findMe);
}

ResultCode Heap_Reproiritize(Heap* self, void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  size_t index;
  ResultCode result_code = FindIndex(self, item, &index);
  if (result_code != kSuccess) return result_code;

  // Index is the first item in queue, so it can only go down
  if (index == 0) return BubbleDown(self, index);

  // Figure out if item has a higher priority than it
  size_t parent_index = ParentIndex(index);
  int comp_result =
      self->comparator(self->data[index], self->data[parent_index]);

  // If they are equal, no need to move anything
  if (comp_result == 0) return kSuccess;

  // If it is greater, then bubble it up
  if (comp_result > 0) return BubbleUp(self, index);

  // The only other option is less so bubble it down
  return BubbleDown(self, index);
}

ResultCode Heap_Delete(Heap* self, void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  size_t index;
  ResultCode result_code = FindIndex(self, item, &index);
  if (result_code != kSuccess) return result_code;

  result_code = ClearIndex(self, item);
  if (result_code != kSuccess) return result_code;

  self->n--;

  // Deleting the last item so nothing needs to be relocated
  if (index == self->n) return kSuccess;

  // Move the last item to the empty space
  result_code = MoveItem(self, self->data[self->n], index);
  if (result_code != kSuccess) return result_code;

  return Heap_Reproiritize(self, self->data[index]);
}

size_t Heap_Size(Heap* self) {
  if (self == NULL) return 0;

  return self->n;
}

size_t Heap_MaxSize(Heap* self) {
  if (self == NULL) return 0;

  return self->size;
}
