#include <stddef.h>

typedef struct test_struct {
    int foo;
    int bar;
    char sorter;
} test_struct_t;

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
