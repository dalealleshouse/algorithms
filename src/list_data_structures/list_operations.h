// Copyright 2020 Hideous Humpback Freak https://hideoushumpbackfreak.com/
#pragma once

#include <stddef.h>
#include <stdio.h>

#include "error_reporter.h"

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
  kkEmptyList = -5,
  kkInvalidIndex = -4,
  kkNotFound = -3,
  kFailedMalloc = -2,
  kkNullParameter = -1,
  kkSuccess = 0
} ListOpResult;

char* ListOp_ErrorMessage(ListOpResult);
