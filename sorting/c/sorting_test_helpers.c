#include <stdlib.h>
#include <string.h>

#include "CUnit/CUnit.h"

#include "sorting_test_helpers.h"

int int_comparator(const void* x, const void* y)
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

int struct_comparator(const void* x, const void* y)
{
    if (x == y)
        return 0;

    if (x == NULL && y != NULL)
        return 0;

    if (y == NULL && x != NULL)
        return 1;

    test_struct_t* _x = (test_struct_t*)x;
    test_struct_t* _y = (test_struct_t*)y;

    return int_comparator(&_x->sorter, &_y->sorter);
}

int pointer_comparator(const void* x, const void* y)
{
    if (x == y)
        return 0;

    if (x == NULL && y != NULL)
        return 0;

    if (y == NULL && x != NULL)
        return 1;

    test_struct_t* _x = *(test_struct_t**)x;
    test_struct_t* _y = *(test_struct_t**)y;

    return int_comparator(&_x->sorter, &_y->sorter);
}

void* dup(const void* ptr, const size_t size)
{
    void* new_ptr = malloc(size);
    memcpy(new_ptr, ptr, size);
    return new_ptr;
}

void arrays_are_equal(
    const size_t n, const size_t size, const void* expected, const void* actual)
{
    char* t_expected = (char*)expected;
    char* t_actual = (char*)actual;

    for (size_t i = 0; i < n; i++)
        CU_ASSERT_EQUAL(
            0, memcmp(t_expected + i * size, t_actual + i * size, size));
}

void* sort_with_c(const size_t n, const size_t size, const void* arr,
    const comparator comparator)
{
    void* new = dup(arr, n * size);
    qsort(new, n, size, comparator);
    return new;
}

void* seq_arr(size_t n)
{
    int* arr = malloc(sizeof(int) * n);
    for (size_t i = 0; i < n; i++)
        arr[i] = i;

    return arr;
}

void* rev_arr(size_t n)
{
    int* arr = malloc(sizeof(int) * n);
    for (size_t i = n; i > 0; i--)
        arr[i - 1] = i;

    return arr;
}

void* rand_arr(size_t n)
{
    int* arr = malloc(sizeof(int) * n);
    for (size_t i = 0; i < n; i++)
        arr[i] = rand();

    return arr;
}

void* rand_st_arr(size_t n)
{
    test_struct_t* arr = malloc(sizeof(test_struct_t) * n);

    for (size_t i = 0; i < n; i++)
        arr[i] = (test_struct_t){ i, i + 1, rand() };

    return arr;
}

void* rand_ptr_arr(size_t n)
{
    test_struct_t** arr = malloc(sizeof(void*) * n);

    for (size_t i = 0; i < n; i++) {
        test_struct_t* tmp = malloc(sizeof(test_struct_t));
        *tmp = (test_struct_t){ i, i + 1, rand() };

        arr[i] = tmp;
    }

    return arr;
}

void* dup_val_arr(size_t n)
{
    int* arr = malloc(sizeof(int) * n);
    for (size_t i = 0; i < n; i++)
        arr[i] = rand() % 10;

    return arr;
}

void free_ptr_arr(size_t n, void** arr)
{
    for (size_t i = 0; i < n; i++)
        free(arr[i]);

    free(arr);
}
