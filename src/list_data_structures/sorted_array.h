// Copyright 2020 Hideous Humpback Freak https://hideoushumpbackfreak.com/
#pragma once

#include "array.h"

ResultCode SortedArray_Search(const Array*, const void*, void**);
ResultCode SortedArray_Min(const Array*, void**);
ResultCode SortedArray_Max(const Array*, void**);
ResultCode SortedArray_Predecessor(const Array*, const void*, void**);
ResultCode SortedArray_Successor(const Array*, const void*, void**);
ResultCode SortedArray_Select(const Array*, const size_t, void**);
ResultCode SortedArray_Rank(const Array*, const void*, size_t*);
