#pragma once

#include <stddef.h>

#include "include/CommonTypes.h"

const size_t INITAL_HEAP_SIZE;

typedef enum {
    RunningMedianArithmeticOverflow = -6,
    RunningMedianEmpty = -5,
    RunningMedianInvalidValue = -4,
    RunningMedianFailedMemoryAllocation = -3,
    RunningMedianNullParamater = -2,
    RunningMedianHeapError = -1,
    RunningMedianSuccess = 0
} RunningMedianResult;

#define RUNNING_MEDIAN_ERROR(result)                                           \
    {                                                                          \
        char str[1000];                                                        \
        sprintf(str, "Running Median: %s, %s, %s, %d\n",                       \
            RunningMedian_ErrorMessage(result), __FILE__, __FUNCTION__,        \
            __LINE__);                                                         \
                                                                               \
        ErrorReporter_Report(result, str);                                     \
    }

typedef struct RunningMedian RunningMedian;

RunningMedian* RunningMedian_Create();
RunningMedianResult RunningMedian_Insert(RunningMedian*, double value);
double RunningMedian_Median(RunningMedian*);
size_t RunningMedian_GetN(RunningMedian*);
void RunningMedian_Destroy(RunningMedian*);
char* RunningMedian_ErrorMessage(RunningMedianResult);
