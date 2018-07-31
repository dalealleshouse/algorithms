#include <stdlib.h>
#include <string.h>

#include "quick_sort.h"

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
int pivot_on_random(const size_t n)
{
    return rand() % n;
}

int _swap(const size_t size, void* x, void* y)
{
    if (size == 0 || x == NULL || y == NULL)
        return -1;

    char n[size];
    memcpy(n, x, size);
    memcpy(x, y, size);
    memcpy(y, n, size);

    return 0;
}

// It is assumed that the pivot will be the first item in the array
int partition(const size_t n, const size_t size, void* arr,
    const comparator comparator, size_t* pivot_index)
{
    if (arr == NULL || comparator == NULL || pivot_index == NULL || n == 0
        || size == 0)
        return -1;

    char* t_arr = (char*)arr;
    char* pivot_value = t_arr;
    t_arr += size;
    char* pivot_pos = t_arr;
    *pivot_index = 0;

    for (size_t i = 1; i < n; i++) {
        // If the item is less, swap it, otherwise do nothing
        if (comparator(t_arr, pivot_value) < 0) {
            if (_swap(size, t_arr, pivot_pos) < 0)
                return -1;

            pivot_pos += size;
            (*pivot_index)++;
        }

        t_arr += size;
    }

    if (*pivot_index > 0)
        if (_swap(size, pivot_value, pivot_value + *pivot_index * size) < 0)
            return -1;

    return 0;
}

int quick_sort(
    const size_t n, const size_t size, void* arr, const comparator comparator)
{
    return quick_sort_pivot(n, size, arr, comparator, pivot_on_random);
}

int quick_sort_pivot(const size_t n, const size_t size, void* arr,
    const comparator comparator, const choose_pivot choose_pivot)
{
    if (arr == NULL || comparator == NULL)
        return -1;

    if (n <= 1)
        return 0;

    size_t pivot_index;
    int pivot;

    if ((pivot = choose_pivot(n)) < 0)
        return -1;

    // move the partition value to the first position
    if (_swap(size, arr, (char*)arr + pivot * size) != 0)
        return -2;

    if (partition(n, size, arr, comparator, &pivot_index) != 0)
        return -3;

    int result;
    // items to the left of the partition
    if (pivot_index > 0)
        if ((result = quick_sort(pivot_index, size, arr, comparator)) != 0)
            return result;

    // items to the right of the partition
    pivot_index++;
    if (pivot_index < n)
        if ((result = quick_sort(n - pivot_index, size,
                 (char*)arr + pivot_index * size, comparator))
            != 0)
            return result;

    return 0;
}
