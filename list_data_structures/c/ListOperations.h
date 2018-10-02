#pragma once

#include <stddef.h>

typedef enum {
    ListOp_InvalidIndex = -4,
    ListOp_NotFound = -3,
    ListOp_FailedMalloc = -2,
    ListOp_NullParameter = -1,
    ListOp_Success = 0
} ListOpResult;
