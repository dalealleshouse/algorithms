#include "./list_operations.h"
#include <stdint.h>

const size_t RANK_ERROR = SIZE_MAX;

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
