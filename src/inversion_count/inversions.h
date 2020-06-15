#pragma once

#include <stdlib.h>

#include "../utils/comparators.h"

unsigned long count_inversions(const void* arr, const size_t length,
                               const size_t item_size, comparator comparator);
