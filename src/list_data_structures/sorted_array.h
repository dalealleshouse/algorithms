/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include "array.h"

ResultCode SortedArray_Search(const Array*, const void*, void**);
ResultCode SortedArray_Min(const Array*, void**);
ResultCode SortedArray_Max(const Array*, void**);
ResultCode SortedArray_Predecessor(const Array*, const void*, void**);
ResultCode SortedArray_Successor(const Array*, const void*, void**);
ResultCode SortedArray_Select(const Array*, const size_t, void**);
ResultCode SortedArray_Rank(const Array*, const void*, size_t*);
