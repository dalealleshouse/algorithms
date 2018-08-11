#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "algo_timer.h"
#include "quick_select.h"

typedef void* (*selector)(const size_t nth, const size_t n, const size_t size,
    void* arr, const comparator comparator);

static int int_comparator(const void* x, const void* y)
{
    if (x == y)
        return 0;

    int _x = *(int*)x;
    int _y = *(int*)y;

    if (_x == _y)
        return 0;

    if (_x < _y)
        return -1;

    return 1;
}

static selector get_selector(const algo algo)
{
    switch (algo) {
    case QUICK_SELECT:
        return quick_select;
    case SORT:
        return sort_select;
    default:
        fprintf(stderr, "Invalid select algorithm\n");
        return NULL;
    }
}

double select_time(const size_t n, const algo algo)
{
    selector selector = get_selector(algo);
    if (selector == NULL)
        return -1;

    int arr[n];
    for (size_t i = 0; i < n; i++)
        arr[i] = rand();

    clock_t t = clock();
    void* result = selector(5, n, sizeof(int), arr, int_comparator);
    t = clock() - t;

    if (result == NULL)
        return -1;

    double time = ((double)t) / CLOCKS_PER_SEC;

    return time;
}
