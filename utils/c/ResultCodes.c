#include "ResultCode.h"

char* Result_ErrorMessage(Result result) {
  switch (result) {
    case OutputPointerIsNotNull:
      return "An output parameter pointer is not null. These are intented to "
             "be populated by the function.";
    case Duplicate:
      return "The paramerter is a duplicate of an existing item";
    case NotFound:
      return "The requested object was not found.";
    case Empty:
      return "List is empty";
    case DependancyError:
      return "Error code from a dependancy";
    case ArgumentOutOfRange:
      return "The specified value is outside the range of valid values";
    case InvalidIndex:
      return "Invalid index";
    case ArithmeticOverflow:
      return "Arithmetic Overflow";
    case FailedMemoryAllocation:
      return "Failed to allocate memory";
    case NullParameter:
      return "A required parameter is NULL";
    case Success:
      return "Success";
    default:
      return "Unknown result code";
  }
}
