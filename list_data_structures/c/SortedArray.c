#include <stdio.h>

#include "SortedArray.h"
#include "include/ErrorReporter.h"

#define ARRAY_ERROR(result)                                                    \
    {                                                                          \
        char str[1000];                                                        \
        sprintf(str, "Heap Error: %s, %s, %s, %d\n",                           \
            ListOp_ErrorMessage(result), __FILE__, __FUNCTION__, __LINE__);    \
                                                                               \
        ErrorReporter_Report(result, str);                                     \
    }

const size_t RANK_ERROR = SIZE_MAX;

static void* BinarySearch(const void* arr, comparator comparator,
    const void* search_for, const size_t n, const size_t item_size)
{
    if (n <= 0) {
        ARRAY_ERROR(ListOp_NotFound);
        return NULL;
    }

    size_t half = n / 2;

    void* tester = (char*)arr + (half * item_size);
    int result = comparator(search_for, tester);

    if (result == 0)
        return tester;

    if (result > 0) {
        void* starting_point = (char*)arr + (item_size * (half + 1));
        size_t right_items = n - half - 1;
        return BinarySearch(
            starting_point, comparator, search_for, right_items, item_size);
    }

    return BinarySearch(arr, comparator, search_for, half, item_size);
}

static size_t FindIndex(const Array* sut, void* ptr)
{
    size_t diff = (char*)ptr - (char*)sut->array;
    return diff / sut->item_size;
}

void* SortedArray_Search(const Array* self, const void* item)
{
    if (self == NULL || item == NULL) {
        ARRAY_ERROR(ListOp_NullParameter);
        return NULL;
    }

    return BinarySearch(
        self->array, self->comparator, item, self->n, self->item_size);
}

void* SortedArray_Min(const Array* self)
{
    if (self == NULL) {
        ARRAY_ERROR(ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        ARRAY_ERROR(ListOp_EmptyList);
        return NULL;
    }

    return self->array;
}

void* SortedArray_Max(const Array* self)
{
    if (self == NULL) {
        ARRAY_ERROR(ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        ARRAY_ERROR(ListOp_EmptyList);
        return NULL;
    }

    return (char*)self->array + (self->item_size * (self->n - 1));
}

void* SortedArray_Predecessor(const Array* self, const void* item)
{
    if (self == NULL || item == NULL) {
        ARRAY_ERROR(ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        ARRAY_ERROR(ListOp_EmptyList);
        return NULL;
    }

    void* found = SortedArray_Search(self, item);
    if (found == NULL)
        return NULL;

    if (found == self->array)
        return NULL;

    return (char*)found - self->item_size;
}

void* SortedArray_Successor(const Array* self, const void* item)
{
    if (self == NULL || item == NULL) {
        ARRAY_ERROR(ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        ARRAY_ERROR(ListOp_EmptyList);
        return NULL;
    }

    void* found = SortedArray_Search(self, item);
    if (found == NULL)
        return NULL;

    if (found == (char*)self->array + ((self->n - 1) * self->item_size))
        return NULL;

    return (char*)found + self->item_size;
}

void* SortedArray_Select(const Array* self, const size_t index)
{
    if (self == NULL) {
        ARRAY_ERROR(ListOp_NullParameter);
        return NULL;
    }

    if (index >= self->n) {
        ARRAY_ERROR(ListOp_InvalidIndex);
        return NULL;
    }

    return (char*)self->array + (index * self->item_size);
}

// Returns RANK_ERROR for item not found or error
size_t SortedArray_Rank(const Array* self, const void* item)
{
    if (self == NULL || item == NULL) {
        ARRAY_ERROR(ListOp_NullParameter);
        return RANK_ERROR;
    }

    if (self->n == 0) {
        ARRAY_ERROR(ListOp_EmptyList);
        return RANK_ERROR;
    }

    void* found = SortedArray_Search(self, item);
    if (found == NULL)
        return RANK_ERROR;

    return FindIndex(self, found);
}
