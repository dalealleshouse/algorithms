#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "algo_timer.h"
#include "bubble_sort.h"
#include "insertion_sort.h"
#include "merge_sort.h"
#include "quick_sort.h"
#include "selection_sort.h"
#include "sorting_test_helpers.h"

typedef int (*sorter)(
    const size_t n, const size_t size, void* arr, const comparator comparator);

static int qsort_adapter(
    const size_t n, const size_t size, void* arr, const comparator comparator)
{
    qsort(arr, n, size, comparator);
    return 0;
}

static int merge_sort_adapter(
    const size_t n, const size_t size, void* arr, const comparator comparator)
{
    char* tmp[n * size];
    int result = merge_sort(arr, tmp, n, size, comparator);

    return result;
}

static sorter get_sorter(const sort_algo algo)
{
    switch (algo) {
    case C:
        return qsort_adapter;
    case BUBBLE:
        return bubble_sort;
    case INSERTION:
        return insertion_sort;
    case SELECTION:
        return selection_sort;
    case QUICK:
        return quick_sort;
    case MERGE:
        return merge_sort_adapter;
    default:
        fprintf(stderr, "Invalid sorting algorithm\n");
        return NULL;
    }
}

static arr_gen get_arr_get(const arr_type type)
{
    switch (type) {
    case RANDOM:
        return rand_arr;
    case DUPLICATES:
        return dup_val_arr;
    case REVERSED:
        return rev_arr;
    case SORTED:
        return seq_arr;
    default:
        fprintf(stderr, "Invalid array type\n");
        return NULL;
    }
}

double sort_time(const size_t n, const arr_type type, const sort_algo algo)
{
    sorter sorter = get_sorter(algo);
    if (sorter == NULL)
        return -1;

    arr_gen arr_gen = get_arr_get(type);
    if (arr_gen == NULL)
        return -1;

    int* arr = arr_gen(n);

    clock_t t = clock();
    int result = sorter(n, sizeof(int), arr, int_comparator);
    t = clock() - t;

    free(arr);
    if (result < 0)
        return -1;

    double time = ((double)t) / CLOCKS_PER_SEC;

    return time;
}
