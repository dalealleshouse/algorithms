#include <stdlib.h>
#include <string.h>

#include "quick_select.h"

int pivot_on_zero(const size_t n)
{
    (void)n;
    return 0;
}

/*
 * This is a flawed implementation because it should be returning a uniformly
 * random number and it's not. However, for the purposes of this project, it's
 * no that important
 */
int pivot_on_random(const size_t n) { return rand() % n; }

// It is assumed that there are never any null values or overlapping memory
// locations. We can assume this b/c this is a private function
static void _swap(const size_t size, void* x, void* y)
{
    char n[size];
    memcpy(n, x, size);
    memcpy(x, y, size);
    memcpy(y, n, size);
}

// It is assumed that the pivot will be the first item in the array
int partition(
    const size_t n, const size_t size, void* arr, const comparator comparator)
{
    if (arr == NULL || comparator == NULL || n == 0 || size == 0)
        return -1;

    int pivot_index = n - 1;
    char* t_arr = (char*)arr;
    char* pivot_value = t_arr;
    char* left_ptr = t_arr + size;
    char* right_ptr = t_arr + size * (n - 1);

    while (1) {
        while (left_ptr < right_ptr && comparator(left_ptr, pivot_value) < 0)
            left_ptr += size;

        while (right_ptr > t_arr && comparator(right_ptr, pivot_value) >= 0) {
            right_ptr -= size;
            pivot_index = pivot_index - 1;
        }

        if (left_ptr >= right_ptr)
            break;
        else
            _swap(size, left_ptr, right_ptr);
    }

    _swap(size, pivot_value, right_ptr);

    return pivot_index;
}

void* quick_select(const size_t nth, const size_t n, const size_t size,
    void* arr, const comparator comparator)
{
    return quick_select_pivot(nth, n, size, arr, comparator, pivot_on_random);
}

void* quick_select_pivot(const size_t nth, const size_t n, const size_t size,
    void* arr, const comparator comparator, const choose_pivot choose_pivot)
{
    if (arr == NULL || comparator == NULL || nth > n - 1)
        return NULL;

    if (n <= 1)
        return arr;

    int pivot_index;
    int pivot;

    if ((pivot = choose_pivot(n)) < 0)
        return NULL;

    // move the partition value to the first position
    _swap(size, arr, (char*)arr + pivot * size);

    if ((pivot_index = partition(n, size, arr, comparator)) < 0)
        return NULL;

    // Normally, it is not a good idea to cast from int to size_t. However, in
    // this case we know pivot_index is positive b/c of the check above, so we
    // can do so without compunction
    if ((size_t)pivot_index == nth)
        return (char*)arr + pivot_index * size;
    else if ((size_t)pivot_index < nth)
        return quick_select_pivot(nth - pivot_index, n - pivot_index, size,
            (char*)arr + pivot_index * size, comparator, choose_pivot);
    else
        return quick_select_pivot(
            nth, pivot_index, size, arr, comparator, choose_pivot);
}

void* sort_select(const size_t nth, const size_t n, const size_t size,
    void* arr, const comparator comparator)
{
    if (arr == NULL || comparator == NULL)
        return NULL;

    if (n <= 1)
        return 0;

    qsort(arr, n, size, comparator);
    return (char*)arr + nth * size;
}

void* max(
    const size_t n, const size_t size, void* arr, const comparator comparator)
{
    if (arr == NULL || comparator == NULL || size == 0 || n == 0)
        return NULL;

    void* max = arr;

    for (size_t i = 1; i < n; i++) {
        void* ith = (char*)arr + i * size;
        if (comparator(max, ith) < 0)
            max = ith;
    }

    return max;
}

void* min(
    const size_t n, const size_t size, void* arr, const comparator comparator)
{
    if (arr == NULL || comparator == NULL || size == 0 || n == 0)
        return NULL;

    void* min = arr;

    for (size_t i = 1; i < n; i++) {
        void* ith = (char*)arr + i * size;
        if (comparator(min, ith) > 0)
            min = ith;
    }

    return min;
}
