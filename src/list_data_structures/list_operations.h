// Copyright 2020 Dale Alleshouse
#pragma once

#include <stddef.h>
#include <stdio.h>

#include "../utils/error_reporter.h"

const size_t RANK_ERROR;

#define LIST_ERROR(list_type, result)                                        \
  {                                                                          \
    char str[1000];                                                          \
    snprintf(str, sizeof(str), "%s Error: %s, %s, %s, %d\n", list_type,      \
             ListOp_ErrorMessage(result), __FILE__, __FUNCTION__, __LINE__); \
                                                                             \
    ErrorReporter_Report(result, str);                                       \
  }

typedef void (*item_handler)(void* x);

typedef enum {
  ListOp_EmptyList = -5,
  ListOp_InvalidIndex = -4,
  ListOp_NotFound = -3,
  ListOp_FailedMalloc = -2,
  ListOp_NullParameter = -1,
  ListOp_Success = 0
} ListOpResult;

char* ListOp_ErrorMessage(ListOpResult);
