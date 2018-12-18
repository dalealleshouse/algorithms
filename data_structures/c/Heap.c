#include <stdlib.h>

#include "Heap.h"
#include "include/MemAllocMock.h"
#include "include/OverflowChecker.h"

static size_t ParentIndex(size_t index)
{
    index++;
    index = index >> 1;
    return index - 1;
}

static size_t ChildIndex(size_t index)
{
    index++;
    index = index << 1;
    return index - 1;
}

static void Swap(void* data[], size_t x, size_t y)
{
    void* temp = data[x];
    data[x] = data[y];
    data[y] = temp;
}

static void BubbleUp(Heap* self, size_t start)
{
    while (start > 0) {
        size_t parent_index = ParentIndex(start);
        int comp_result
            = self->comparator(self->data[start], self->data[parent_index]);

        if (comp_result <= 0)
            break;

        Swap(self->data, start, parent_index);
        start = parent_index;
    }
}

static size_t GreatestPriority(Heap* self, size_t x, size_t y)
{
    if (y >= self->n)
        return x;

    if (x >= self->n)
        return y;

    int result = self->comparator(self->data[x], self->data[y]);

    if (result >= 0)
        return x;

    return y;
}

static void BubbleDown(Heap* self, size_t start)
{
    size_t child;

    while ((child = ChildIndex(start)) < self->n) {
        child = GreatestPriority(self, child, child + 1);

        int comp_result
            = self->comparator(self->data[start], self->data[child]);

        if (comp_result >= 0)
            return;

        Swap(self->data, start, child);
        start = child;
    }
}

Heap* Heap_Create(size_t size, comparator comparator)
{
    if (comparator == NULL) {
        HEAP_ERROR(HeapNullParameter);
        return NULL;
    }

    if (size == 0) {
        HEAP_ERROR(HeapInvalidSize);
        return NULL;
    }

    if (is_mul_overflow_size_t(sizeof(void*), size)) {
        HEAP_ERROR(HeapArithmeticOverflow);
        return NULL;
    }

    Heap* self = malloc(sizeof(Heap));

    if (self == NULL) {
        HEAP_ERROR(HeapFailedMemoryAllocation);
        return NULL;
    }

    self->data = malloc(sizeof(void*) * size);

    if (self->data == NULL) {
        HEAP_ERROR(HeapFailedMemoryAllocation);
        Heap_Destroy(self, NULL);
        return NULL;
    }

    self->size = size;
    self->n = 0;
    self->comparator = comparator;

    return self;
}

void Heap_Destroy(Heap* self, freer freer)
{
    if (self == NULL)
        return;

    if (freer != NULL && self->data != NULL) {
        for (size_t i = 0; i < self->n; i++)
            freer(self->data[i]);
    }

    free(self->data);
    free(self);
}

HeapResult Heap_Insert(Heap* self, void* item)
{
    if (self == NULL || item == NULL)
        return HeapNullParameter;

    if (self->size == self->n)
        return HeapOverflow;

    self->data[self->n] = item;
    BubbleUp(self, self->n);
    self->n++;

    return HeapSuccess;
}

HeapResult Heap_Resize(Heap* self, size_t size)
{
    if (self == NULL)
        return HeapNullParameter;

    if (size < self->n)
        return HeapInvalidSize;

    if (is_mul_overflow_size_t(size, sizeof(void*)))
        return HeapArithmeticOverflow;

    void* new_data = realloc(self->data, sizeof(void*) * size);
    if (new_data == NULL)
        return HeapFailedMemoryAllocation;

    self->data = new_data;
    self->size = size;

    return HeapSuccess;
}

void* Heap_Extract(Heap* self)
{
    if (self == NULL) {
        HEAP_ERROR(HeapNullParameter);
        return NULL;
    }

    if (Heap_IsEmpty(self)) {
        HEAP_ERROR(HeapEmpty);
        return NULL;
    }

    void* item = self->data[0];
    self->n--;
    Swap(self->data, 0, self->n);
    BubbleDown(self, 0);

    return item;
}

void* Heap_Find(Heap* self)
{
    if (self == NULL) {
        HEAP_ERROR(HeapNullParameter);
        return NULL;
    }

    if (Heap_IsEmpty(self)) {
        HEAP_ERROR(HeapEmpty);
        return NULL;
    }

    return self->data[0];
}

bool Heap_IsEmpty(Heap* self)
{
    if (self == NULL) {
        HEAP_ERROR(HeapNullParameter);
        return true;
    }

    if (self->n == 0)
        return true;

    return false;
}

char* Heap_ErrorMessage(HeapResult result)
{
    switch (result) {
    case HeapArithmeticOverflow:
        return "An operation caused an arithmetic overflow";
    case HeapOverflow:
        return "Attempted to place more items in the heap than it is sized for";
    case HeapEmpty:
        return "Attempted operation on an empty heap";
    case HeapFailedMemoryAllocation:
        return "Failed to allocate memory";
    case HeapInvalidSize:
        return "n must be greater than zero.";
    case HeapNullParameter:
        return "One of the required parameters passed to the function is NULL";
    case HeapSuccess:
        return "Success";
    default:
        return "Unknown error code";
    }
}
