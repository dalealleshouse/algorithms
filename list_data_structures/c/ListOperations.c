#include <stdint.h>

#include "ListOperations.h"

const size_t RANK_ERROR = SIZE_MAX;

int int_comparator(const void* x, const void* y) {
  if (x == y) return 0;

  int _x = *(int*)x;
  int _y = *(int*)y;

  if (_x == _y) return 0;

  if (_x < _y) return -1;

  return 1;
}

char* ListOp_ErrorMessage(ListOpResult result) {
  switch (result) {
    case ListOp_EmptyList:
      return "The list is empty";
    case ListOp_InvalidIndex:
      return "Index is outside of bounds";
    case ListOp_NotFound:
      return "Item not found in list";
    case ListOp_FailedMalloc:
      return "Failed to allocate memory";
    case ListOp_NullParameter:
      return "One of the required parameters passed to the function is NULL";
    case ListOp_Success:
      return "Success";
    default:
      return "Unknown error code";
  }
}
