#pragma once

#include <stddef.h>

typedef int (*comparator)(const void* x, const void* y);
typedef void (*freer)(void* x);

// Compators
int PIntComparator(const void* x, const void* y);
