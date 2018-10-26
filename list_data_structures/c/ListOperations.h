#pragma once

#include <stddef.h>

typedef void (*freer)(void* x);
typedef void (*item_handler)(void* x);
typedef int (*comparator)(const void* x, const void* y);

int int_comparator(const void*, const void*);

typedef enum {
    ListOp_InvalidIndex = -4,
    ListOp_NotFound = -3,
    ListOp_FailedMalloc = -2,
    ListOp_NullParameter = -1,
    ListOp_Success = 0
} ListOpResult;
