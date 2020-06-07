#include <stdbool.h>
#include <stdlib.h>

typedef struct {
  bool active;
  unsigned intercept_count;
  void* mock_value;
} Interception;

static Interception _malloc_intercept = {false, 0, NULL};

void MemAllocMock_InterceptMalloc(void* ptr) {
  _malloc_intercept.active = true;
  _malloc_intercept.mock_value = ptr;
}

unsigned MemAllocMock_MallocInterceptCount() {
  return _malloc_intercept.intercept_count;
}

void MemAllocMock_ResetMalloc() {
  _malloc_intercept.active = false;
  _malloc_intercept.intercept_count = 0;
}

void* mock_malloc(size_t size) {
  if (_malloc_intercept.active == true) {
    _malloc_intercept.intercept_count++;
    return _malloc_intercept.mock_value;
  }

  return malloc(size);
}

void* mock_calloc(size_t nmemb, size_t size) {
  if (_malloc_intercept.active == true) {
    _malloc_intercept.intercept_count++;
    return _malloc_intercept.mock_value;
  }

  return calloc(nmemb, size);
}

void* mock_realloc(void* ptr, size_t size) {
  if (_malloc_intercept.active == true) {
    _malloc_intercept.intercept_count++;
    return _malloc_intercept.mock_value;
  }

  return realloc(ptr, size);
}
