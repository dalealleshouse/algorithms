/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/

#include "cache.h"

#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "hash_table.h"
#include "heap.h"
#include "malloc_test_wrapper.h"
#include "test_helpers.h"

typedef struct Cache {
  size_t limit;
  HashTable* data;
  Heap* time_tracker;
  freer freer;
} Cache;

static unsigned int stub_count = 0;
static char* last_key = NULL;

#define SUT(limit, code_block)                                \
  {                                                           \
    stub_count = 0;                                           \
    Cache* sut = NULL;                                        \
    ResultCode result_code = Cache_Create(limit, free, &sut); \
    CU_ASSERT_EQUAL(result_code, kSuccess);                   \
                                                              \
    code_block;                                               \
                                                              \
    Cache_Destroy(sut);                                       \
  }

static void Cache_Create_null_parameter(void) {
  Cache* sut = NULL;

  ResultCode result_code = Cache_Create(5, free, NULL);
  CU_ASSERT_EQUAL(result_code, kNullParameter);

  sut = malloc(1);
  result_code = Cache_Create(5, free, &sut);
  CU_ASSERT_EQUAL(result_code, kOutputPointerIsNotNull);
  free(sut);
}

static void Cache_Create_failed_malloc(void) {
#if !defined(NDEBUG)
  FAILED_MALLOC_TEST({
    Cache* sut = NULL;
    ResultCode result_code = Cache_Create(5, free, &sut);
    CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
  });
#endif
}

static void Cache_Create_invalid_parameter(void) {
  Cache* sut = NULL;

  ResultCode result_code = Cache_Create(0, free, &sut);
  CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);

  free(sut);
}

static void Cache_Create_initalizes_values(void) {
  const size_t n = 100;

  SUT(n, {
    CU_ASSERT_EQUAL(sut->limit, n);
    CU_ASSERT_PTR_EQUAL(sut->freer, free);
  });
}

static void* ProducerStub(const void* key, const size_t key_size) {
  stub_count++;

  char* key_t = (char*)key;
  char* cache_value = malloc(key_size + 1);
  strncpy(cache_value, key_t, key_size + 1);
  last_key = cache_value;

  return cache_value;
}

static void Cache_Get_null_parameter(void) {
  const char* test_key = "not null... yet";
  void* result = NULL;

  SUT(10, {
    result_code = Cache_Get(NULL, (void*)test_key, strlen(test_key),
                            ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code =
        Cache_Get(sut, (void*)test_key, strlen(test_key), NULL, &result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code =
        Cache_Get(sut, (void*)test_key, strlen(test_key), ProducerStub, NULL);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result = malloc(1);
    result_code = Cache_Get(sut, (void*)test_key, strlen(test_key),
                            ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kOutputPointerIsNotNull);

    free(result);
  });
}

static void Cache_Get_invalid_parameter(void) {
  const char* test_key = "not null... yet";
  void* result = NULL;

  SUT(10, {
    result_code = Cache_Get(sut, (void*)test_key, 0, ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);
  });
}

static void Cache_Get_calls_producer_when_key_not_cached(void) {
  const size_t n = 100;
  const char* test_key = "I am a key";
  void* result = NULL;
  SUT(n, {
    ResultCode result_code = Cache_Get(sut, (void*)test_key, strlen(test_key),
                                       ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(stub_count, 1);
    CU_ASSERT_STRING_EQUAL(last_key, test_key);

    CU_ASSERT_EQUAL(HashTable_GetN(sut->data), 1);
    CU_ASSERT_STRING_EQUAL(result, test_key);
  });
}

static void Cache_Get_does_not_call_producer_when_key_cached(void) {
  const size_t n = 100;
  const char* test_key = "I am a key";
  void* result = NULL;
  SUT(n, {
    ResultCode result_code = Cache_Get(sut, (void*)test_key, strlen(test_key),
                                       ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(stub_count, 1);
    CU_ASSERT_STRING_EQUAL(last_key, test_key);

    result = NULL;
    result_code = Cache_Get(sut, (void*)test_key, strlen(test_key),
                            ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(stub_count, 1);
    CU_ASSERT_STRING_EQUAL(last_key, test_key);

    CU_ASSERT_EQUAL(HashTable_GetN(sut->data), 1);
    CU_ASSERT_STRING_EQUAL(result, test_key);
  });
}

static void Cache_Get_caches_multiple_items(void) {
  const size_t n = 100;
  const char* test_key1 = "I am a key 1";
  const char* test_key2 = "I am a key 2";
  const char* test_key3 = "I am a key 3";

  void* result = NULL;
  SUT(n, {
    ResultCode result_code = Cache_Get(sut, (void*)test_key1, strlen(test_key1),
                                       ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(stub_count, 1);
    CU_ASSERT_EQUAL(HashTable_GetN(sut->data), 1);
    CU_ASSERT_STRING_EQUAL(last_key, test_key1);

    result = NULL;
    result_code = Cache_Get(sut, (void*)test_key2, strlen(test_key2),
                            ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(stub_count, 2);
    CU_ASSERT_EQUAL(HashTable_GetN(sut->data), 2);
    CU_ASSERT_STRING_EQUAL(last_key, test_key2);

    result = NULL;
    result_code = Cache_Get(sut, (void*)test_key3, strlen(test_key3),
                            ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(stub_count, 3);
    CU_ASSERT_EQUAL(HashTable_GetN(sut->data), 3);
    CU_ASSERT_STRING_EQUAL(last_key, test_key3);
  });
}

static void Cache_Get_caches_clears_oldest_items(void) {
  const size_t n = 2;
  const size_t nkeys = 10;

  char* keys[] = {"key 1",
                  "test key 2",
                  "another key",
                  "so many keys",
                  "cant get enough",
                  "key madness",
                  "just key",
                  "foo",
                  "bar",
                  "fop"};
  SUT(n, {
    void* result = NULL;
    ResultCode result_code =
        Cache_Get(sut, (void*)keys[0], strlen(keys[0]), ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(stub_count, 1);
    CU_ASSERT_EQUAL(HashTable_GetN(sut->data), 1);
    CU_ASSERT_STRING_EQUAL(last_key, keys[0]);

    result = NULL;
    result_code =
        Cache_Get(sut, (void*)keys[1], strlen(keys[1]), ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(stub_count, 2);
    CU_ASSERT_EQUAL(HashTable_GetN(sut->data), 2);
    CU_ASSERT_STRING_EQUAL(last_key, keys[1]);

    for (size_t i = 2; i < nkeys; i++) {
      result = NULL;
      result_code = Cache_Get(sut, (void*)keys[i], strlen(keys[i]),
                              ProducerStub, &result);
      CU_ASSERT_EQUAL(result_code, kSuccess);
      CU_ASSERT_EQUAL(stub_count, i + 1);
      CU_ASSERT_EQUAL(HashTable_GetN(sut->data), 2);
      CU_ASSERT_STRING_EQUAL(last_key, keys[i]);
      CU_ASSERT_STRING_EQUAL(result, keys[i]);

      CU_ASSERT_FALSE(HashTable_KeyExists(sut->data, (void*)keys[i - 2],
                                          strlen(keys[i - 2])));
    }
  });
}

static void Cache_Get_caches_updates_last_access(void) {
  const size_t n = 5;

  char* keys[] = {"key 1",
                  "test key 2",
                  "another key",
                  "so many keys",
                  "cant get enough",
                  "key madness",
                  "just key",
                  "foo",
                  "bar",
                  "fop"};
  SUT(n, {
    void* result = NULL;

    // Load 5 items
    for (size_t i = 0; i < 5; i++) {
      result = NULL;
      result_code = Cache_Get(sut, (void*)keys[i], strlen(keys[i]),
                              ProducerStub, &result);
      CU_ASSERT_EQUAL(result_code, kSuccess);
      CU_ASSERT_EQUAL(stub_count, i + 1);
      CU_ASSERT_EQUAL(HashTable_GetN(sut->data), i + 1);
      CU_ASSERT_STRING_EQUAL(last_key, keys[i]);
      CU_ASSERT_STRING_EQUAL(result, keys[i]);
    }

    // Update last access on all of them, index 4 should be oldest item now
    for (size_t i = 5; i > 0; i--) {
      result = NULL;
      result_code = Cache_Get(sut, (void*)keys[i - 1], strlen(keys[i - 1]),
                              ProducerStub, &result);
      CU_ASSERT_EQUAL(result_code, kSuccess);
      CU_ASSERT_EQUAL(stub_count, 5);
      CU_ASSERT_EQUAL(HashTable_GetN(sut->data), 5);
      CU_ASSERT_STRING_EQUAL(result, keys[i - 1]);
    }

    result = NULL;
    result_code =
        Cache_Get(sut, (void*)keys[5], strlen(keys[5]), ProducerStub, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(stub_count, 6);
    CU_ASSERT_EQUAL(HashTable_GetN(sut->data), 5);
    CU_ASSERT_STRING_EQUAL(result, keys[5]);

    CU_ASSERT_FALSE(
        HashTable_KeyExists(sut->data, (void*)keys[4], strlen(keys[4])));
  });
}
int RegisterCacheTests(void) {
  CU_TestInfo cache_tests[] = {
      CU_TEST_INFO(Cache_Create_null_parameter),
      CU_TEST_INFO(Cache_Create_failed_malloc),
      CU_TEST_INFO(Cache_Create_initalizes_values),
      CU_TEST_INFO(Cache_Create_invalid_parameter),
      CU_TEST_INFO(Cache_Get_null_parameter),
      CU_TEST_INFO(Cache_Get_invalid_parameter),
      CU_TEST_INFO(Cache_Get_calls_producer_when_key_not_cached),
      CU_TEST_INFO(Cache_Get_does_not_call_producer_when_key_cached),
      CU_TEST_INFO(Cache_Get_caches_multiple_items),
      CU_TEST_INFO(Cache_Get_caches_clears_oldest_items),
      CU_TEST_INFO(Cache_Get_caches_updates_last_access),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Cache",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = cache_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
