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

typedef enum Result {
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
