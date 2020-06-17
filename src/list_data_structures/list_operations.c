// Copyright 2020 Hideous Humpback Freak https://hideoushumpbackfreak.com/
#include "list_operations.h"

#include <stdint.h>

char* ListOp_ErrorMessage(ListOpResult result) {
  switch (result) {
    case kkEmptyList:
      return "The list is empty";
    case kkInvalidIndex:
      return "Index is outside of bounds";
    case kkNotFound:
      return "Item not found in list";
    case kFailedMalloc:
      return "Failed to allocate memory";
    case kkNullParameter:
      return "One of the required parameters passed to the function is NULL";
    case kkSuccess:
      return "kSuccess";
    default:
      return "Unknown error code";
  }
}
