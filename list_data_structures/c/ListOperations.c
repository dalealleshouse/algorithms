#include "ListOperations.h"

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
