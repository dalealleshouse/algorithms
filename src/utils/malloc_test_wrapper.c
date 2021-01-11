/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include <stdbool.h>
#include <unistd.h>

static bool intercept_malloc = false;
static size_t intercept_count = 0;
// NOLINTNEXTLINE(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
void* __real_malloc(size_t size);

// NOLINTNEXTLINE(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
void* __wrap_malloc(size_t size) {
  if (intercept_malloc) {
    ++intercept_count;
    return NULL;
  }

  return __real_malloc(size);
}

// NOLINTNEXTLINE(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
void* __real_calloc(size_t nmemb, size_t size);

// NOLINTNEXTLINE(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
void* __wrap_calloc(size_t nmemb, size_t size) {
  if (intercept_malloc) {
    ++intercept_count;
    return NULL;
  }

  return __real_calloc(nmemb, size);
}

// NOLINTNEXTLINE(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
void* __real_realloc(void* ptr, size_t size);

// NOLINTNEXTLINE(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
void* __wrap_realloc(void* ptr, size_t size) {
  if (intercept_malloc) {
    ++intercept_count;
    return NULL;
  }

  return __real_realloc(ptr, size);
}

void InterceptMalloc() { intercept_malloc = true; }
void ResetMalloc() {
  intercept_count = 0;
  intercept_malloc = false;
}

size_t MallocInterceptCount() { return intercept_count; }
