#pragma once

#include <stddef.h>

typedef enum { QUICK_SELECT = 1, QSORT_SELECT = 2 } algo;

typedef enum { MAX = 1, MIN = 2 } linear_algo;

double select_time(const size_t n, const size_t nth, const algo algo);
double linear_time(const size_t n, const linear_algo algo);
