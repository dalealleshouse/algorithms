/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "hash_table.h"

#include <inttypes.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "malloc_test_wrapper.h"
#include "test_helpers.h"

const size_t n = 16384;
static const double kEpsilon = 0.0000000000001;

#define SUT(code_block)                                 \
  {                                                     \
    HashTable* sut = NULL;                              \
    ResultCode result_code = HashTable_Create(n, &sut); \
    CU_ASSERT_EQUAL(result_code, kSuccess);             \
    code_block;                                         \
    HashTable_Destroy(sut, NULL);                       \
  }

static void VerifyGet(HashTable* sut, void* key, size_t size, void* expected) {
  void* actual = NULL;
  ResultCode result_code = HashTable_Get(sut, key, size, &actual);
  CU_ASSERT_EQUAL(kSuccess, result_code);
  CU_ASSERT_PTR_EQUAL(expected, actual);
}

static void HashTable_Create_failed_malloc() {
#if !defined(NDEBUG)
  FAILED_MALLOC_TEST({
    HashTable* result = NULL;
    ResultCode result_code = HashTable_Create(8, &result);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
  });
#endif
}

static void HashTable_Create_invalid_size() {
  HashTable* sut = NULL;
  ResultCode result_code = HashTable_Create(0, &sut);
  CU_ASSERT_PTR_NULL(sut);
  CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);
}

static void HashTable_Create_happy_path() {
  HashTable* sut = NULL;
  ResultCode result_code = HashTable_Create(16384, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);
  CU_ASSERT_PTR_NOT_NULL(sut);
  HashTable_Destroy(sut, NULL);
}

static void HashTable_Put_null_parameter() {
  char* key = "we are";
  int value = 138;

  Result result = HashTable_Put(NULL, NULL, 10, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result);

  result = HashTable_Put(NULL, key, strlen(key), &value);
  CU_ASSERT_EQUAL(kNullParameter, result);

  SUT({
    result = HashTable_Put(sut, NULL, 10, &value);
    CU_ASSERT_EQUAL(kNullParameter, result);

    result = HashTable_Put(sut, key, strlen(key), NULL);
    CU_ASSERT_EQUAL(kNullParameter, result);
  });
}

static void HashTable_Put_failed_malloc() {
  char* key = "we are";
  int value = 138;

  SUT({
    FAILED_MALLOC_TEST({
      Result result = HashTable_Put(sut, key, strlen(key), &value);
      CU_ASSERT_EQUAL(result, kFailedMemoryAllocation);
    });
  });
}

static void HashTable_Put_happy_path() {
  char* key = "we are";
  int value = 138;

  SUT({
    CU_ASSERT_EQUAL(0, HashTable_GetN(sut));
    Result result = HashTable_Put(sut, key, strlen(key), &value);
    CU_ASSERT_EQUAL(kSuccess, result);
    CU_ASSERT_EQUAL(1, HashTable_GetN(sut));

    void* found = NULL;
    ResultCode result_code = HashTable_Get(sut, key, strlen(key), &found);
    CU_ASSERT_EQUAL(kSuccess, result_code);
    CU_ASSERT_PTR_EQUAL(found, &value);
  });
}

static void HashTable_Put_chains() {
  int *one = malloc(sizeof(int)), *two = malloc(sizeof(int)),
      *three = malloc(sizeof(int));
  char *one_key = "one", *two_key = "two", *three_key = "three";
  HashTable* sut = NULL;
  ResultCode result_code = HashTable_Create(2, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  HashTable_Put(sut, one_key, strlen(one_key), one);
  HashTable_Put(sut, two_key, strlen(two_key), two);
  HashTable_Put(sut, three_key, strlen(three_key), three);

  CU_ASSERT_EQUAL(HashTable_GetLoadFactor(sut), 1.5);
  VerifyGet(sut, one_key, strlen(one_key), one);
  VerifyGet(sut, two_key, strlen(two_key), two);
  VerifyGet(sut, three_key, strlen(three_key), three);

  HashTable_Destroy(sut, free);
}

static void HashTable_Put_updates_existing() {
  char* key = "we are";
  int first = 138;
  int second = 1380138;
  SUT({
    HashTable_Put(sut, key, strlen(key), &first);
    VerifyGet(sut, key, strlen(key), &first);
    /* CU_ASSERT_EQUAL(HashTable_Get(sut, key, strlen(key)), &first); */
    CU_ASSERT_EQUAL(HashTable_GetN(sut), 1);

    HashTable_Put(sut, key, strlen(key), &second);
    VerifyGet(sut, key, strlen(key), &second);
    /* CU_ASSERT_EQUAL(HashTable_Get(sut, key, strlen(key)), &second); */
    CU_ASSERT_EQUAL(HashTable_GetN(sut), 1);
  });
}

static void HashTable_Remove_null_parameter() {
  char* key = "we are";

  Result result = HashTable_Remove(NULL, NULL, 10);
  CU_ASSERT_EQUAL(kNullParameter, result);

  result = HashTable_Remove(NULL, key, 10);
  CU_ASSERT_EQUAL(kNullParameter, result);

  SUT({
    result = HashTable_Remove(sut, NULL, 10);
    CU_ASSERT_EQUAL(kNullParameter, result);
  });
}

static void HashTable_Remove_not_found() {
  int value = 5;
  char* key = "some key";
  char* key2 = "some other key";
  char* key3 = "yet another";
  char* key4 = "401 not found";

  SUT({
    HashTable_Put(sut, key, strlen(key), &value);
    HashTable_Put(sut, key2, strlen(key), &value);
    HashTable_Put(sut, key3, strlen(key), &value);
    CU_ASSERT_EQUAL(HashTable_GetN(sut), 3);

    Result result = HashTable_Remove(sut, key4, strlen(key4));
    CU_ASSERT_EQUAL(result, kNotFound);
    CU_ASSERT_EQUAL(HashTable_GetN(sut), 3);
  });
}

static void HashTable_Remove_happy_path() {
  char* key = "we are";
  char* key2 = "some key";
  char* key3 = "some other key";
  char* key4 = "yet another";
  int value = 138;

  SUT({
    HashTable_Put(sut, key2, strlen(key2), &value);
    HashTable_Put(sut, key3, strlen(key3), &value);
    HashTable_Put(sut, key4, strlen(key4), &value);
    HashTable_Put(sut, key, strlen(key), &value);
    CU_ASSERT_EQUAL(HashTable_GetN(sut), 4);

    Result result = HashTable_Remove(sut, key, strlen(key));
    CU_ASSERT_EQUAL(result, kSuccess);
    CU_ASSERT_EQUAL(HashTable_GetN(sut), 3);
    void* found = NULL;
    HashTable_Get(sut, key, strlen(key), &found);
    CU_ASSERT_PTR_NULL(found);
  });
}

static void HashTable_GetLoadFactor_null_parameter() {
  double result = HashTable_GetLoadFactor(NULL);
  CU_ASSERT(isnan(result));
}

static void HashTable_GetLoadFactor_happy_path() {
  const size_t num_keys = 94;
  int value = 69;

  SUT({
    double result = HashTable_GetLoadFactor(sut);
    CU_ASSERT_DOUBLE_EQUAL(result, 0, kEpsilon);

    char key[2] = "!";
    for (size_t i = 0; i < num_keys; i++) {
      HashTable_Put(sut, key, strlen(key), &value);
      key[0]++;
    }

    CU_ASSERT_EQUAL(HashTable_GetN(sut), num_keys);

    result = HashTable_GetLoadFactor(sut);
    double expected = (double)num_keys / (double)n;
    CU_ASSERT_DOUBLE_EQUAL(result, expected, kEpsilon);
  });
}

static void HashTable_GetN_null_paramter() {
  size_t result = HashTable_GetN(NULL);
  CU_ASSERT_EQUAL(result, 0);
}

static void HashTable_Get_null_parameter() {
  char* key = "42";

  void* result = NULL;
  ResultCode result_code = HashTable_Get(NULL, NULL, 10, &result);
  CU_ASSERT_EQUAL(kNullParameter, result_code);
  CU_ASSERT_EQUAL(NULL, result);

  result_code = HashTable_Get(NULL, key, 10, &result);
  CU_ASSERT_EQUAL(kNullParameter, result_code);
  CU_ASSERT_EQUAL(NULL, result);

  SUT({
    result_code = HashTable_Get(sut, NULL, 10, &result);
    CU_ASSERT_EQUAL(NULL, result);
    CU_ASSERT_EQUAL(kNullParameter, result_code);
  });
}

static void HashTable_Get_not_found() {
  char* not_found = "401";
  SUT({
    void* result = NULL;
    ResultCode result_code =
        HashTable_Get(sut, not_found, strlen(not_found), &result);
    CU_ASSERT_EQUAL(kNotFound, result_code);
    CU_ASSERT_PTR_NULL(result);
  });
}

static void HashTable_Get_happy_path() {
  int one = 1;
  int two = 2;

  char* key_one = "one";
  char* key_two = "two";

  SUT({
    HashTable_Put(sut, key_one, strlen(key_one), &one);
    HashTable_Put(sut, key_two, strlen(key_two), &two);

    VerifyGet(sut, key_one, strlen(key_one), &one);
    VerifyGet(sut, key_two, strlen(key_two), &two);
  });
}

static void HashTable_Destroy_null() { HashTable_Destroy(NULL, NULL); }

static void HashTable_Destroy_frees_items() {
  int* one = malloc(sizeof(int));
  int* two = malloc(sizeof(int));
  char* key_one = "one";
  char* key_two = "two";

  // failures will get caught by address sanitizer
  HashTable* sut = NULL;
  ResultCode result_code = HashTable_Create(n, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  HashTable_Put(sut, key_one, strlen(key_one), one);
  HashTable_Put(sut, key_two, strlen(key_two), two);
  HashTable_Destroy(sut, free);
}

static void HashTable_int_keys() {
  int key = 138;
  char* value = "we are";
  int key2 = 138138;
  char* value2 = "we are, we are";

  SUT({
    Result result = HashTable_Put(sut, &key, sizeof(int), value);
    CU_ASSERT_EQUAL(result, kSuccess);
    result = HashTable_Put(sut, &key2, sizeof(int), value2);
    CU_ASSERT_EQUAL(result, kSuccess);

    VerifyGet(sut, &key, sizeof(int), value);
    VerifyGet(sut, &key2, sizeof(int), value2);
  });
}

static void HashTable_long_keys() {
  int64_t key = 138;
  char* value = "we are";
  int64_t key2 = -3;
  char* value2 = "we are, we are";

  SUT({
    Result result = HashTable_Put(sut, &key, sizeof(int64_t), value);
    CU_ASSERT_EQUAL(result, kSuccess);
    result = HashTable_Put(sut, &key2, sizeof(int64_t), value2);
    CU_ASSERT_EQUAL(result, kSuccess);

    VerifyGet(sut, &key, sizeof(int64_t), value);
    VerifyGet(sut, &key2, sizeof(int64_t), value2);
  });
}

static void HashTable_GetCollisions_null_parameter() {
  size_t result = HashTable_GetCollisions(NULL);
  CU_ASSERT_EQUAL(result, 0);
}

static void HashTable_GetCollisions_happy_path() {
  int *one = malloc(sizeof(int)), *two = malloc(sizeof(int)),
      *three = malloc(sizeof(int));
  char *one_key = "one", *two_key = "two", *three_key = "three";
  HashTable* sut = NULL;
  ResultCode result_code = HashTable_Create(2, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  HashTable_Put(sut, one_key, strlen(one_key), one);
  HashTable_Put(sut, two_key, strlen(two_key), two);
  HashTable_Put(sut, three_key, strlen(three_key), three);

  CU_ASSERT_EQUAL(HashTable_GetCollisions(sut), 1);

  HashTable_Destroy(sut, free);
}

static void HashTable_KeyExists_null_parameter() {
  char* key = "we are";

  bool result = HashTable_KeyExists(NULL, NULL, 10);
  CU_ASSERT_FALSE(result);

  result = HashTable_KeyExists(NULL, key, 10);
  CU_ASSERT_FALSE(result);

  SUT({
    result = HashTable_KeyExists(sut, NULL, 10);
    CU_ASSERT_FALSE(result);
  });
}

static void HashTable_KeyExists_returns_true_when_item_exists() {
  char* key = "we are";
  int value = 138;

  SUT({
    HashTable_Put(sut, key, strlen(key), &value);

    bool found = HashTable_KeyExists(sut, key, strlen(key));
    CU_ASSERT_TRUE(found);
  });
}

static void HashTable_KeyExists_returns_false_when_item_does_not_exist() {
  char* key = "we are";

  SUT({
    bool found = HashTable_KeyExists(sut, key, strlen(key));
    CU_ASSERT_FALSE(found);
  });
}

static void HashTable_Enumerate_null_parameter() {
  Result result = HashTable_Enumerate(NULL, NULL, NULL);
  CU_ASSERT_EQUAL(result, kNullParameter);
}

static void _itemHandler(const KeyValuePair* item, const size_t index,
                         void* context) {
  *(int*)context += (*(int*)item->value + (int)index);
}

static void HashTable_Enumerate_happy_path() {
  char* key = "we are";
  int value = 138;
  char* key2 = "we areeeeeee";
  int value2 = 13888888;
  int context = 0;

  SUT({
    HashTable_Put(sut, key, strlen(key), &value);
    HashTable_Put(sut, key2, strlen(key2), &value2);

    HashTable_Enumerate(sut, _itemHandler, &context);

    // results should be (order not guaranteed)
    // iteration 1 = 138 + 0
    // iteration 2 = 13888888 + 1
    CU_ASSERT_EQUAL(context, value + value2 + 1);
  });
}

int RegisterHashTableTests() {
  CU_TestInfo hash_table_tests[] = {
      CU_TEST_INFO(HashTable_Create_failed_malloc),
      CU_TEST_INFO(HashTable_Create_invalid_size),
      CU_TEST_INFO(HashTable_Create_happy_path),
      CU_TEST_INFO(HashTable_Put_null_parameter),
      CU_TEST_INFO(HashTable_Put_failed_malloc),
      CU_TEST_INFO(HashTable_Put_happy_path),
      CU_TEST_INFO(HashTable_Put_chains),
      CU_TEST_INFO(HashTable_Put_updates_existing),
      CU_TEST_INFO(HashTable_Remove_not_found),
      CU_TEST_INFO(HashTable_Remove_null_parameter),
      CU_TEST_INFO(HashTable_Remove_happy_path),
      CU_TEST_INFO(HashTable_GetLoadFactor_null_parameter),
      CU_TEST_INFO(HashTable_GetLoadFactor_happy_path),
      CU_TEST_INFO(HashTable_GetN_null_paramter),
      CU_TEST_INFO(HashTable_Get_null_parameter),
      CU_TEST_INFO(HashTable_Get_not_found),
      CU_TEST_INFO(HashTable_Get_happy_path),
      CU_TEST_INFO(HashTable_Destroy_null),
      CU_TEST_INFO(HashTable_Destroy_frees_items),
      CU_TEST_INFO(HashTable_int_keys),
      CU_TEST_INFO(HashTable_long_keys),
      CU_TEST_INFO(HashTable_GetCollisions_null_parameter),
      CU_TEST_INFO(HashTable_GetCollisions_happy_path),
      CU_TEST_INFO(HashTable_KeyExists_null_parameter),
      CU_TEST_INFO(HashTable_KeyExists_returns_true_when_item_exists),
      CU_TEST_INFO(HashTable_KeyExists_returns_false_when_item_does_not_exist),
      CU_TEST_INFO(HashTable_Enumerate_null_parameter),
      CU_TEST_INFO(HashTable_Enumerate_happy_path),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Hash Table",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = hash_table_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
