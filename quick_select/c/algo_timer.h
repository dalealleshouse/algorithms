#pragma once

#include <stddef.h>

typedef enum {
    QUICK_SELECT = 1,
    SORT = 2,
} algo;

double select_time(const size_t n, const algo algo);
