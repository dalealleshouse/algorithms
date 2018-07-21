#include <string.h>

#include "quick_sort.h"

int _choose_pivot(const size_t n, const size_t size, void* arr)
{
    (void)size;
    (void)arr;

    return 0;
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
        int result = comparator(t_arr, pivot_value);

        if (result < 0) {
            int result = _swap(size, t_arr, pivot_pos);
            if (result < 0)
                return result;

            pivot_pos += size;
            (*pivot_index)++;
        }

        t_arr += size;
    }

    if (*pivot_index > 0) {
        int result
            = _swap(size, pivot_value, pivot_value + *pivot_index * size);

        if (result < 0)
            return result;
    }

    return 0;
}

int quick_sort(
    const size_t n, const size_t size, void* arr, const comparator comparator)
{
    if (arr == NULL || comparator == NULL)
        return -1;

    if (n <= 1)
        return 0;

    size_t pivot_index;
    int pivot;

    if ((pivot = _choose_pivot(n, size, arr)) < 0)
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
