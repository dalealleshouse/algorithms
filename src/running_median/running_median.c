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
#include "linked_list.h"

const size_t kInitialHeapSize = 100;

typedef struct RunningMedian {
  Heap* upper;
  Heap* lower;
  LinkedList* sliding_values;
  size_t n;
  size_t sliding_window;
} RunningMedian;

static bool IsBalanced(RunningMedian* self) {
  return (self->n % 2 == 0) ? Heap_Size(self->upper) == Heap_Size(self->lower)
                            : imaxabs((intmax_t)(Heap_Size(self->upper) -
                                                 Heap_Size(self->lower))) == 1;
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

// WARNING: Makes sure to re-balance the heaps after running this
static ResultCode MaintainSlidingWindow(RunningMedian* self,
                                        median_value* val) {
  // Use a linked list to track the sliding values
  // new items are always inserted at the front, the oldest items will always be
  // at the back. This makes it easy to know which item to delete

  // Insert new values at the head
  LinkedList_InsertAt(self->sliding_values, val, 0);

  if (self->n > self->sliding_window) {
    // Remove values stored at the tail
    median_value* doomed = self->sliding_values->tail->payload;

    // Figure out which heap the doomed value is in
    median_value* low_limit = NULL;
    ResultCode result_code = Heap_Peek(self->lower, (void**)&low_limit);
    if (result_code != kSuccess) return result_code;

    Heap* h = (*doomed <= *low_limit) ? self->lower : self->upper;

    // Delete the value that's outside the sliding window
    result_code = Heap_Delete(h, doomed);
    if (result_code != kSuccess) return result_code;
    self->n--;

    // remove the tail value
    result_code =
        LinkedList_DeleteAt(self->sliding_values, self->sliding_window);
    if (result_code != kSuccess) return result_code;

    free(doomed);
  }

  return kSuccess;
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

ResultCode RunningMedian_Create(RunningMedian** result, size_t sliding_window) {
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

  rm->sliding_values = NULL;
  rm->sliding_window = sliding_window;
  if (rm->sliding_window > 0) {
    result_code = LinkedList_Create(NULL, NULL, &rm->sliding_values);
  }

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

  if (self->n == 0) {
    // If no items exist, just put it on the lower heap
    result_code = Heap_Insert(self->lower, val);
    if (result_code != kSuccess) goto fail;
  } else {
    // Figure out which heap to push the new value on
    median_value* median = NULL;
    result_code = Heap_Peek(self->lower, (void**)&median);
    if (result_code != kSuccess) goto fail;

    Heap* h = (*val < *median) ? self->lower : self->upper;

    // THIS IS WHERE THE ERROR WAS THROWN
    result_code = Heap_Insert(h, val);
    if (result_code != kSuccess) goto fail;
  }

  self->n++;

  if (self->sliding_window > 0) {
    result_code = MaintainSlidingWindow(self, val);
    if (result_code != kSuccess) goto fail;
  }

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
  LinkedList_Destroy(self->sliding_values);
  free(self);
}
