#include "./error_reporter.h"

#include <stdlib.h>
#include <string.h>

const static size_t ERROR_MSG_SIZE = 1000;
static char _msg[1000];
static int _error_code = 0;

void ErrorReporter_Report(int error_code, char* msg) {
  _error_code = error_code;

  if (msg != NULL) strncpy(_msg, msg, ERROR_MSG_SIZE);  // NOLINT
}

int ErrorReporter_LastErrorCode() { return _error_code; }

char* ErrorReporter_LastErrorMessage() { return _msg; }

void ErrorReporter_Clear() {
  _error_code = 0;
  _msg[0] = '\0';
  /* memset(_msg, 0, ERROR_MSG_SIZE); */
}
