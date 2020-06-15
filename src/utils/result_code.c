#include "./result_code.h"

char* Result_ErrorMessage(Result result) {
  switch (result) {
    case kSystemError:
      return "Underlying OS error";
    case kSecurityError:
      return "There was a security related error";
    case kOutputPointerIsNotNull:
      return "An output parameter pointer is not null. These are intented to "
             "be populated by the function.";
    case kDuplicate:
      return "The paramerter is a duplicate of an existing item";
    case kNotFound:
      return "The requested object was not found.";
    case kEmpty:
      return "List is empty";
    case kDependancyError:
      return "Error code from a dependancy";
    case kArgumentOutOfRange:
      return "The specified value is outside the range of valid values";
    case kInvalidIndex:
      return "Invalid index";
    case kArithmeticOverflow:
      return "Arithmetic Overflow";
    case kFailedMemoryAllocation:
      return "Failed to allocate memory";
    case kNullParameter:
      return "A required parameter is NULL";
    case kSuccess:
      return "kSuccess";
    default:
      return "Unknown result code";
  }
}
