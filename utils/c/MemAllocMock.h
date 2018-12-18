#pragma once

#include <stddef.h>

void MemAllocMock_InterceptMalloc(void*);
unsigned MemAllocMock_MallocInterceptCount();
void MemAllocMock_ResetMalloc();

void* mock_malloc(size_t size);
void* mock_calloc(size_t nmemb, size_t size);
void* mock_realloc(void* ptr, size_t size);

#define malloc(size) mock_malloc(size)
#define calloc(nmemb, size) mock_calloc(nmemb, size)
#define realloc(ptr, size) mock_realloc(ptr, size)
