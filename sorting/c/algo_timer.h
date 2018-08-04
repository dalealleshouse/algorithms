#pragma once

#include <stddef.h>

typedef enum {
    C = 1,
    BUBBLE = 2,
    INSERTION = 3,
    SELECTION = 4,
    QUICK = 5,
    MERGE = 6
} sort_algo;

typedef enum { RANDOM = 1, SORTED = 2, REVERSED = 3, DUPLICATES = 4 } arr_type;

double sort_time(const size_t n, const arr_type type, const sort_algo algo);
