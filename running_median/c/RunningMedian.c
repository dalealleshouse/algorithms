#include <math.h>
#include <stdlib.h>

#include "RunningMedian.h"
#include "include/Heap.h"
#include "include/MemAllocMock.h"

const size_t INITAL_HEAP_SIZE = 500;

typedef struct RunningMedian {
    Heap* upper;
    Heap* lower;
    size_t n;
} RunningMedian;

static bool is_even(size_t n) { return n % 2 == 0; }

static int max_comparator(const void* x, const void* y)
{
    double _x = *(double*)x;
    double _y = *(double*)y;

    if (_x == _y)
        return 0;

    if (_x < _y)
        return -1;

    return 1;
}

static int min_comparator(const void* x, const void* y)
{
    double _x = *(double*)x;
    double _y = *(double*)y;

    if (_x == _y)
        return 0;

    if (_x > _y)
        return -1;

    return 1;
}

static double get_heap_value(Heap* heap)
{
    void* val = Heap_Find(heap);
    if (val == NULL) {
        RUNNING_MEDIAN_ERROR(RunningMedianHeapError);
        return NAN;
    }

    return *(double*)val;
}

static bool heap_is_balanced(RunningMedian* self)
{
    if (is_even(self->n))
        return self->upper->n == self->lower->n;
    else
        return self->upper->n + 1 == self->lower->n;
}

static HeapResult heap_insert(Heap* heap, double* value)
{
    HeapResult result;

    if (heap->n == heap->size) {
        result = Heap_Resize(heap, heap->size * 2);
        if (result != HeapSuccess)
            return result;
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
static RunningMedianResult balance_heaps(RunningMedian* self)
{
    if (heap_is_balanced(self))
        return RunningMedianSuccess;

    Heap* too_many;
    Heap* too_low;

    if (self->upper->n > self->lower->n) {
        too_many = self->upper;
        too_low = self->lower;
    } else {
        too_many = self->lower;
        too_low = self->upper;
    }

    double* temp = Heap_Extract(too_many);
    if (temp == NULL)
        return RunningMedianHeapError;

    HeapResult result = heap_insert(too_low, temp);
    if (result != HeapSuccess)
        return RunningMedianHeapError;
    return RunningMedianSuccess;
}

static Heap* find_insert_heap(RunningMedian* self, double value)
{
    if (self->n == 0)
        return self->lower;

    double low_value = get_heap_value(self->lower);
    if (isnan(low_value))
        return NULL;

    if (value > low_value)
        return self->upper;
    else
        return self->lower;
}

RunningMedian* RunningMedian_Create()
{
    RunningMedian* rm = malloc(sizeof(RunningMedian));
    if (rm == NULL) {
        RUNNING_MEDIAN_ERROR(RunningMedianFailedMemoryAllocation);
        return NULL;
    }

    rm->upper = Heap_Create(INITAL_HEAP_SIZE, min_comparator);
    if (rm->upper == NULL) {
        RUNNING_MEDIAN_ERROR(RunningMedianHeapError);
        RunningMedian_Destroy(rm);
        return NULL;
    }

    rm->lower = Heap_Create(INITAL_HEAP_SIZE, max_comparator);
    if (rm->lower == NULL) {
        RUNNING_MEDIAN_ERROR(RunningMedianHeapError);
        RunningMedian_Destroy(rm);
        return NULL;
    }

    rm->n = 0;
    return rm;
}

RunningMedianResult RunningMedian_Insert(RunningMedian* self, double value)
{
    if (self == NULL)
        return RunningMedianNullParamater;

    if (isnan(value) || isinf(value))
        return RunningMedianInvalidValue;

    double* val = malloc(sizeof(double));
    if (val == NULL)
        return RunningMedianFailedMemoryAllocation;

    *val = value;

    // Find which heap to insert the value into
    Heap* h = find_insert_heap(self, value);
    if (h == NULL)
        return RunningMedianHeapError;

    // Insert the value
    HeapResult hresult = heap_insert(h, val);
    if (hresult != HeapSuccess)
        return RunningMedianHeapError;

    self->n++;

    // Rebalance
    return balance_heaps(self);
}

size_t RunningMedian_GetN(RunningMedian* self)
{
    if (self == NULL)
        return 0;

    return self->n;
}

double RunningMedian_Median(RunningMedian* self)
{
    if (self == NULL) {
        RUNNING_MEDIAN_ERROR(RunningMedianNullParamater);
        return NAN;
    }

    if (self->n == 0) {
        RUNNING_MEDIAN_ERROR(RunningMedianEmpty);
        return NAN;
    }

    if (is_even(self->n)) {
        double low = get_heap_value(self->lower);
        if (isnan(low))
            return NAN;

        double high = get_heap_value(self->upper);
        if (isnan(high))
            return NAN;

        double result = (low + high) / 2;
        if (isinf(result))
            RUNNING_MEDIAN_ERROR(RunningMedianArithmeticOverflow);

        return result;
    } else
        return get_heap_value(self->lower);

    return 0;
}

void RunningMedian_Destroy(RunningMedian* self)
{
    if (self == NULL)
        return;

    Heap_Destroy(self->upper, free);
    Heap_Destroy(self->lower, free);
    free(self);
}

char* RunningMedian_ErrorMessage(RunningMedianResult result)
{
    switch (result) {
    case RunningMedianArithmeticOverflow:
        return "Running Median - an arithmetic overflow occured";
    case RunningMedianEmpty:
        return "Running Medain - no values have been added yet";
    case RunningMedianInvalidValue:
        return "Running Median - value is either NaN or Infinity";
    case RunningMedianFailedMemoryAllocation:
        return "RunningMedian - Failed Memory Allocation";
    case RunningMedianNullParamater:
        return "RunningMedian - Required parameter is NULL";
    case RunningMedianHeapError:
        return "RunningMedian - One of the internal heaps returned an "
               "non-success code";
    case RunningMedianSuccess:
        return "RunningMedian - Success";
    default:
        return "Unknown error code";
    }
}
