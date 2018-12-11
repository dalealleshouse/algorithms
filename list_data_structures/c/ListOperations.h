#pragma once

#include <stddef.h>
#include <stdio.h>

#include "include/ErrorReporter.h"

#define LIST_ERROR(list_type, result)                                          \
    {                                                                          \
        char str[1000];                                                        \
        sprintf(str, "%s Error: %s, %s, %s, %d\n", list_type,                  \
            ListOp_ErrorMessage(result), __FILE__, __FUNCTION__, __LINE__);    \
                                                                               \
        ErrorReporter_Report(result, str);                                     \
    }

typedef void (*freer)(void* x);
typedef void (*item_handler)(void* x);
typedef int (*comparator)(const void* x, const void* y);

int int_comparator(const void*, const void*);

typedef enum {
    ListOp_EmptyList = -5,
    ListOp_InvalidIndex = -4,
    ListOp_NotFound = -3,
    ListOp_FailedMalloc = -2,
    ListOp_NullParameter = -1,
    ListOp_Success = 0
} ListOpResult;

char* ListOp_ErrorMessage(ListOpResult);
