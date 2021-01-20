/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>

#include "comparators.h"
#include "result_code.h"

extern const size_t INITAL_HEAP_SIZE;

typedef struct RunningMedian RunningMedian;
typedef double median_value;

ResultCode RunningMedian_Create(RunningMedian**);
ResultCode RunningMedian_Insert(RunningMedian*, median_value value);
ResultCode RunningMedian_Median(RunningMedian*, median_value*);
size_t RunningMedian_GetN(RunningMedian*);
void RunningMedian_Destroy(RunningMedian*);
