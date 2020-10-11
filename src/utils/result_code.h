/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stdio.h>

#include "./error_reporter.h"

// We may need a better way to log errors, but this will work for now
#define ERROR(source, result)                                                \
  {                                                                          \
    char str[1000];                                                          \
    snprintf(str, sizeof(str), "%s: %s, %s, %s, %d\n", source,               \
             Result_ErrorMessage(result), __FILE__, __FUNCTION__, __LINE__); \
                                                                             \
    ErrorReporter_Report(result, str);                                       \
  }

#define PRINT_ERROR(source, result)                                         \
  {                                                                         \
    fprintf(stderr, "%s: %s, %s, %s, %d\n", source,                         \
            Result_ErrorMessage(result), __FILE__, __FUNCTION__, __LINE__); \
  }

typedef enum Result {
  kUnderflow = -14,
  kOverflow = -13,
  kSystemError = -12,
  kSecurityError = -11,
  kOutputPointerIsNotNull = -10,
  kDuplicate = -9,
  kNotFound = -8,
  kEmpty = -7,
  kDependancyError = -6,
  kArgumentOutOfRange = -5,
  kInvalidIndex = -4,
  kArithmeticOverflow = -3,
  kFailedMemoryAllocation = -2,
  kNullParameter = -1,
  kSuccess = 0
} Result;

typedef Result ResultCode;

char* Result_ErrorMessage(Result);
