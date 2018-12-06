#pragma once

#include <stdint.h>

#include "Array.h"

const size_t RANK_ERROR;

void* SortedArray_Search(const Array*, const void*);
void* SortedArray_Min(const Array*);
void* SortedArray_Max(const Array*);
void* SortedArray_Predecessor(const Array*, const void*);
void* SortedArray_Successor(const Array*, const void*);
void* SortedArray_Select(const Array*, const size_t);
size_t SortedArray_Rank(const Array*, const void*);
