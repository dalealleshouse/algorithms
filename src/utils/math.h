#pragma once

#include <limits.h>
#include <stddef.h>
#include "./common.h"

int Min(const size_t count, ...);
double MinDouble(const size_t count, ...);

void* MaxPArray(const size_t n, const size_t size, void* arr,
                const comparator comparator);
void* MinPArray(const size_t n, const size_t size, void* arr,
                const comparator comparator);
