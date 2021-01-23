/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "running_median.h"

#include <inttypes.h>
#include <math.h>
#include <stdlib.h>

#include "heap.h"

const size_t kInitialHeapSize = 100;

typedef struct RunningMedian {
  Heap* upper;
  Heap* lower;
  size_t n;
} RunningMedian;

static bool IsBalanced(RunningMedian* self) {
  return (self->n % 2 == 0)
             ? Heap_Size(self->upper) == Heap_Size(self->lower)
             : imaxabs(Heap_Size(self->upper) - Heap_Size(self->lower)) == 1;
}

// Resize the heaps if there isn't enough room for the insert
static ResultCode ResizeHeapsIfNeeded(RunningMedian* self) {
  ResultCode result_code;

  size_t heap_size = Heap_MaxSize(self->lower) * 2;
  if (heap_size == self->n + 1) {
    result_code = Heap_Resize(self->lower, heap_size);
    if (result_code != kSuccess) return result_code;

    result_code = Heap_Resize(self->upper, heap_size);
    if (result_code != kSuccess) return result_code;
  }

  return kSuccess;
}

// Balance so there is the same number of items on each heap
// It should never be out of balance my more than 1
static ResultCode BalanceHeaps(RunningMedian* self) {
  while (!IsBalanced(self)) {
    Heap* larger = NULL;
    Heap* smaller = NULL;
    if (Heap_Size(self->upper) > Heap_Size(self->lower)) {
      larger = self->upper;
      smaller = self->lower;
    } else {
      larger = self->lower;
      smaller = self->upper;
    }

    void* temp = NULL;
    ResultCode result_code = Heap_Extract(larger, &temp);
    if (result_code != kSuccess) return result_code;

    result_code = Heap_Insert(smaller, temp);
    if (result_code != kSuccess) return result_code;
  }

  return kSuccess;
}

static int MaxComparator(const void* x, const void* y) {
  median_value _x = *(median_value*)x;
  median_value _y = *(median_value*)y;

  if (_x == _y) return 0;
  if (_x < _y) return -1;
  return 1;
}

static int MinComparator(const void* x, const void* y) {
  return MaxComparator(x, y) * -1;
}

ResultCode RunningMedian_Median(RunningMedian* self, median_value* result) {
  ResultCode result_code;

  if (self == NULL) return kNullParameter;
  if (self->n == 0) return kEmpty;

  median_value* median = NULL;
  // If there is an uneven number of items, there will be one extra item on one
  // of the heaps - that will be the median value
  if (self->n % 2 != 0) {
    Heap* h = (Heap_Size(self->lower) > Heap_Size(self->upper)) ? self->lower
                                                                : self->upper;

    result_code = Heap_Peek(h, (void**)&median);
    if (result_code != kSuccess) return result_code;

    *result = *median;
    return kSuccess;
  }

  // If there is an even number of items, then return the average of the item at
  // the top of both heaps
  median_value* median2 = NULL;

  result_code = Heap_Peek(self->lower, (void**)&median);
  if (result_code != kSuccess) return result_code;

  result_code = Heap_Peek(self->upper, (void**)&median2);
  if (result_code != kSuccess) return result_code;

  *result = (*median + *median2) / 2;
  if (!isnormal(*result) && *result != 0) return kArithmeticOverflow;

  return kSuccess;
}

size_t RunningMedian_GetN(RunningMedian* self) {
  if (self == NULL) return 0;

  return self->n;
}

ResultCode RunningMedian_Create(RunningMedian** result) {
  ResultCode result_code = kSuccess;

  if (result == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;

  RunningMedian* rm = malloc(sizeof(RunningMedian));
  if (rm == NULL) return kFailedMemoryAllocation;

  rm->upper = NULL;
  result_code = Heap_Create(kInitialHeapSize, MinComparator, &rm->upper);
  if (result_code != kSuccess) goto error;

  rm->lower = NULL;
  result_code = Heap_Create(kInitialHeapSize, MaxComparator, &rm->lower);
  if (result_code != kSuccess) goto error;

  rm->n = 0;
  *result = rm;
  return result_code;

error:
  RunningMedian_Destroy(rm);
  return result_code;
}

ResultCode RunningMedian_Insert(RunningMedian* self, median_value value) {
  if (self == NULL) return kNullParameter;
  if (!isnormal(value) && value != 0) return kArgumentOutOfRange;

  ResultCode result_code = ResizeHeapsIfNeeded(self);
  if (result_code != kSuccess) return result_code;

  // Heap only holds pointers, so malloc up a pointer to hold the value
  median_value* val = malloc(sizeof(median_value));
  if (val == NULL) return kFailedMemoryAllocation;

  *val = value;

  // If not item exist, just put it on the lower heap
  if (self->n == 0) {
    result_code = Heap_Insert(self->lower, val);
    if (result_code != kSuccess) goto fail;
    self->n++;
    return result_code;
  }

  // Figure out which heap to push the new value on
  median_value* median = NULL;
  result_code = Heap_Peek(self->lower, (void**)&median);
  if (result_code != kSuccess) goto fail;

  Heap* h = (*val < *median) ? self->lower : self->upper;

  // Insert the value
  result_code = Heap_Insert(h, val);
  if (result_code != kSuccess) goto fail;

  self->n++;

  // Balance the heaps if they need it
  result_code = BalanceHeaps(self);
  if (result_code != kSuccess) goto fail;

  return kSuccess;

fail:
  free(val);
  return result_code;
}

void RunningMedian_Destroy(RunningMedian* self) {
  if (self == NULL) return;

  Heap_Destroy(self->upper, free);
  Heap_Destroy(self->lower, free);
  free(self);
}
