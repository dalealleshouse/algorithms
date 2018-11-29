#include <stdlib.h>

#include "Heap.h"
#include "include/MemAllocMock.h"

static size_t ParentIndex(size_t index) { return (index - 1) >> 1; }

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

    // TODO: Add overflow protection
    Heap* self = malloc(sizeof(Heap) + (sizeof(void*) * size));

    if (self == NULL) {
        HEAP_ERROR(HeapFailedMemoryAllocation);
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

    if (freer != NULL) {
        for (size_t i = 0; i < self->n; i++)
            freer(self->data[i]);
    }

    free(self);
}

HeapResult Heap_Insert(Heap* self, void* item)
{
    if (self == NULL || item == NULL)
        return HeapNullParameter;

    self->data[self->n] = item;
    BubbleUp(self, self->n);
    self->n++;

    return HeapSuccess;
}

char* Heap_ErrorMessage(HeapResult result)
{
    switch (result) {
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
