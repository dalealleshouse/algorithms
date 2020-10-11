#include "./running_median.h"

#include <math.h>
#include <stdlib.h>

#include "../data_structures/heap.h"

const size_t INITAL_HEAP_SIZE = 500;

typedef struct RunningMedian {
  Heap* upper;
  Heap* lower;
  size_t n;
} RunningMedian;

static bool is_even(size_t n) { return n % 2 == 0; }

static int max_comparator(const void* x, const void* y) {
  double _x = *(double*)x;
  double _y = *(double*)y;

  if (_x == _y) return 0;

  if (_x < _y) return -1;

  return 1;
}

static int min_comparator(const void* x, const void* y) {
  double _x = *(double*)x;
  double _y = *(double*)y;

  if (_x == _y) return 0;

  if (_x > _y) return -1;

  return 1;
}

static double get_heap_value(Heap* heap) {
  void* val = NULL;
  ResultCode result_code = Heap_Peek(heap, &val);
  if (result_code != kSuccess) {
    ERROR("RunningMedian", result_code);
    return NAN;
  }

  return *(double*)val;
}

static bool heap_is_balanced(RunningMedian* self) {
  if (is_even(self->n)) {
    return Heap_Size(self->upper) == Heap_Size(self->lower);
  } else {
    return Heap_Size(self->upper) + 1 == Heap_Size(self->lower);
  }
}

static ResultCode heap_insert(Heap* heap, double* value) {
  ResultCode result_code;

  if (Heap_Size(heap) == Heap_MaxSize(heap)) {
    result_code = Heap_Resize(heap, Heap_MaxSize(heap) * 2);
    if (result_code != kSuccess) return result_code;
  }

  return Heap_Insert(heap, value);
}

/*
 * The assumption is that the heaps will never be more than one out of balance
 * becuase it should be run on every insert operation
 *
 * if there are an even number of items, there should be an equal number of
 * items in each heap
 *
 * if there are an odd number of items, there should be one more item in lower
 * than in upper
 */
static Result balance_heaps(RunningMedian* self) {
  if (heap_is_balanced(self)) return kSuccess;

  Heap* too_many;
  Heap* too_low;

  if (Heap_Size(self->upper) > Heap_Size(self->lower)) {
    too_many = self->upper;
    too_low = self->lower;
  } else {
    too_many = self->lower;
    too_low = self->upper;
  }

  double* temp = NULL;
  ResultCode result_code = Heap_Extract(too_many, (void**)&temp);
  if (result_code != kSuccess) return result_code;

  result_code = heap_insert(too_low, temp);
  if (result_code != kSuccess) return result_code;

  return kSuccess;
}

static Heap* find_insert_heap(RunningMedian* self, double value) {
  if (self->n == 0) return self->lower;

  double low_value = get_heap_value(self->lower);
  if (isnan(low_value)) return NULL;

  if (value > low_value) {
    return self->upper;
  } else {
    return self->lower;
  }
}

RunningMedian* RunningMedian_Create() {
  RunningMedian* rm = malloc(sizeof(RunningMedian));
  if (rm == NULL) {
    ERROR("RunningMedian", kFailedMemoryAllocation);
    return NULL;
  }

  rm->upper = NULL;
  ResultCode result_code =
      Heap_Create(INITAL_HEAP_SIZE, min_comparator, &rm->upper);
  if (result_code != kSuccess) {
    ERROR("RunningMedian", kDependancyError);
    RunningMedian_Destroy(rm);
    return NULL;
  }

  rm->lower = NULL;
  result_code = Heap_Create(INITAL_HEAP_SIZE, max_comparator, &rm->lower);
  if (result_code != kSuccess) {
    ERROR("RunningMedian", kDependancyError);
    RunningMedian_Destroy(rm);
    return NULL;
  }

  rm->n = 0;
  return rm;
}

Result RunningMedian_Insert(RunningMedian* self, double value) {
  if (self == NULL) return kNullParameter;

  if (isnan(value) || isinf(value)) return kArgumentOutOfRange;

  double* val = malloc(sizeof(double));
  if (val == NULL) return kFailedMemoryAllocation;

  *val = value;

  // Find which heap to insert the value into
  Heap* h = find_insert_heap(self, value);
  if (h == NULL) {
    free(val);
    return kDependancyError;
  }

  // Insert the value
  ResultCode hresult = heap_insert(h, val);
  if (hresult != kSuccess) {
    free(val);
    return hresult;
  }

  self->n++;

  // Rebalance
  return balance_heaps(self);
}

size_t RunningMedian_GetN(RunningMedian* self) {
  if (self == NULL) return 0;

  return self->n;
}

double RunningMedian_Median(RunningMedian* self) {
  if (self == NULL) {
    ERROR("RunningMedian", kNullParameter);
    return NAN;
  }

  if (self->n == 0) {
    ERROR("RunningMedian", kEmpty);
    return NAN;
  }

  if (is_even(self->n)) {
    double low = get_heap_value(self->lower);
    if (isnan(low)) return NAN;

    double high = get_heap_value(self->upper);
    if (isnan(high)) return NAN;

    double result = (low + high) / 2;
    if (isinf(result)) ERROR("RunningMedian", kArithmeticOverflow);

    return result;
  } else {
    return get_heap_value(self->lower);
  }

  return 0;
}

void RunningMedian_Destroy(RunningMedian* self) {
  if (self == NULL) return;

  Heap_Destroy(self->upper, free);
  Heap_Destroy(self->lower, free);
  free(self);
}
