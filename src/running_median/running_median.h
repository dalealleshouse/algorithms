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

/*
 * Allocates and initializes a RunningMedian object
 *
 * params:
 *  <sliding_window> number of items to track (RunningMedian_Median will always
 *                   return the median of the last <sliding_window> inserted
 *                   items)
 *  <self>           output parameter that will be set to the address of the
 *                   newly allocated object
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode RunningMedian_Create(RunningMedian** self, size_t sliding_window);

/*
 * Inserts a new item to list of values
 *
 * params:
 *  <value> value to insert
 *  <self>  initialized RunningMedian object
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode RunningMedian_Insert(RunningMedian*, median_value value);

/*
 * Inserts a new item to list of values
 *
 * params:
 *  <value> value to insert
 *  <self>  initialized RunningMedian object
 *
 * returns:
 *  Result code indicating kSuccess or failure code
 */
ResultCode RunningMedian_Median(RunningMedian*, median_value*);
size_t RunningMedian_GetN(RunningMedian*);
void RunningMedian_Destroy(RunningMedian*);
