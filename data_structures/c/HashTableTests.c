#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "HashTable.h"
#include "include/ErrorReporter.h"
#include "include/TestHelpers.h"

const size_t n = 16384;
const double EPSILON = 0.0000000000001;

#define SUT(code_block)                                                        \
    {                                                                          \
        HashTable* sut = HashTable_Create(n);                                  \
        code_block;                                                            \
        HashTable_Destroy(sut, NULL);                                          \
    }

static void HashTable_Create_failed_malloc()
{
    FAILED_MALLOC_TEST({
        HashTable* result = HashTable_Create(8);
        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(FailedMemoryAllocation, ErrorReporter_LastErrorCode());
    });
}

static void HashTable_Create_invalid_size()
{
    ErrorReporter_Clear();
    HashTable* sut = HashTable_Create(0);
    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(ArgumentOutOfRange, ErrorReporter_LastErrorCode());

    // size is not a power of 2
    ErrorReporter_Clear();
    sut = HashTable_Create(10000);
    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(ArgumentOutOfRange, ErrorReporter_LastErrorCode());
}

static void HashTable_Create_happy_path()
{
    HashTable* sut = HashTable_Create(16384);
    CU_ASSERT_PTR_NOT_NULL(sut);
    HashTable_Destroy(sut, NULL);
}

static void HashTable_Insert_null_parameter()
{
    char* key = "we are";
    int value = 138;

    Result result = HashTable_Insert(NULL, NULL, 10, NULL);
    CU_ASSERT_EQUAL(NullParameter, result);

    result = HashTable_Insert(NULL, key, strlen(key) + 1, &value);
    CU_ASSERT_EQUAL(NullParameter, result);

    SUT({
        result = HashTable_Insert(sut, NULL, 10, &value);
        CU_ASSERT_EQUAL(NullParameter, result);

        result = HashTable_Insert(sut, key, strlen(key) + 1, NULL);
        CU_ASSERT_EQUAL(NullParameter, result);
    });
}

static void HashTable_Insert_failed_malloc()
{
    char* key = "we are";
    int value = 138;

    SUT({
        FAILED_MALLOC_TEST({
            Result result = HashTable_Insert(sut, key, strlen(key) + 1, &value);
            CU_ASSERT_EQUAL(result, FailedMemoryAllocation);
        });
    });
}

static void HashTable_Insert_happy_path()
{
    char* key = "we are";
    int value = 138;

    SUT({
        CU_ASSERT_EQUAL(0, HashTable_GetN(sut));
        Result result = HashTable_Insert(sut, key, strlen(key) + 1, &value);
        CU_ASSERT_EQUAL(Success, result);
        CU_ASSERT_EQUAL(1, HashTable_GetN(sut));

        void* found = HashTable_Find(sut, key, strlen(key) + 1);
        CU_ASSERT_PTR_EQUAL(found, &value);
    });
}

static void HashTable_Insert_chains()
{
    int *one = malloc(sizeof(int)), *two = malloc(sizeof(int)),
        *three = malloc(sizeof(int));
    char *one_key = "one", *two_key = "two", *three_key = "three";
    HashTable* sut = HashTable_Create(2);

    HashTable_Insert(sut, one_key, strlen(one_key) + 1, one);
    HashTable_Insert(sut, two_key, strlen(two_key) + 1, two);
    HashTable_Insert(sut, three_key, strlen(three_key) + 1, three);

    CU_ASSERT_EQUAL(HashTable_GetLoadFactor(sut), 1.5);
    CU_ASSERT_PTR_EQUAL(HashTable_Find(sut, one_key, strlen(one_key) + 1), one);
    CU_ASSERT_PTR_EQUAL(HashTable_Find(sut, two_key, strlen(two_key) + 1), two);
    CU_ASSERT_PTR_EQUAL(
        HashTable_Find(sut, three_key, strlen(three_key) + 1), three);

    HashTable_Destroy(sut, free);
}

static void HashTable_Insert_updates_existing()
{
    char* key = "we are";
    int first = 138;
    int second = 1380138;
    SUT({
        HashTable_Insert(sut, key, strlen(key) + 1, &first);
        CU_ASSERT_EQUAL(HashTable_Find(sut, key, strlen(key) + 1), &first);
        CU_ASSERT_EQUAL(HashTable_GetN(sut), 1);

        HashTable_Insert(sut, key, strlen(key) + 1, &second);
        CU_ASSERT_EQUAL(HashTable_Find(sut, key, strlen(key) + 1), &second);
        CU_ASSERT_EQUAL(HashTable_GetN(sut), 1);
    });
}

static void HashTable_Delete_null_parameter()
{
    char* key = "we are";

    Result result = HashTable_Delete(NULL, NULL, 10);
    CU_ASSERT_EQUAL(NullParameter, result);

    result = HashTable_Delete(NULL, key, 10);
    CU_ASSERT_EQUAL(NullParameter, result);

    SUT({
        result = HashTable_Delete(sut, NULL, 10);
        CU_ASSERT_EQUAL(NullParameter, result);
    });
}

static void HashTable_Delete_not_found()
{
    int value = 5;
    char* key = "some key";
    char* key2 = "some other key";
    char* key3 = "yet another";
    char* key4 = "401 not found";

    SUT({
        HashTable_Insert(sut, key, strlen(key) + 1, &value);
        HashTable_Insert(sut, key2, strlen(key) + 1, &value);
        HashTable_Insert(sut, key3, strlen(key) + 1, &value);
        CU_ASSERT_EQUAL(HashTable_GetN(sut), 3);

        Result result = HashTable_Delete(sut, key4, strlen(key4) + 1);
        CU_ASSERT_EQUAL(result, NotFound);
        CU_ASSERT_EQUAL(HashTable_GetN(sut), 3);
    });
}

static void HashTable_Delete_happy_path()
{
    char* key = "we are";
    char* key2 = "some key";
    char* key3 = "some other key";
    char* key4 = "yet another";
    int value = 138;

    SUT({
        HashTable_Insert(sut, key2, strlen(key2) + 1, &value);
        HashTable_Insert(sut, key3, strlen(key3) + 1, &value);
        HashTable_Insert(sut, key4, strlen(key4) + 1, &value);
        HashTable_Insert(sut, key, strlen(key) + 1, &value);
        CU_ASSERT_EQUAL(HashTable_GetN(sut), 4);

        Result result = HashTable_Delete(sut, key, strlen(key) + 1);
        CU_ASSERT_EQUAL(result, Success);
        CU_ASSERT_EQUAL(HashTable_GetN(sut), 3);
        void* found = HashTable_Find(sut, key, strlen(key) + 1);
        CU_ASSERT_PTR_NULL(found);
    });
}

static void HashTable_GetLoadFactor_null_parameter()
{
    ErrorReporter_Clear();
    double result = HashTable_GetLoadFactor(NULL);
    CU_ASSERT(isnan(result));
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);
}

static void HashTable_GetLoadFactor_happy_path()
{
    const size_t num_keys = 94;
    int value = 69;

    SUT({
        double result = HashTable_GetLoadFactor(sut);
        CU_ASSERT_DOUBLE_EQUAL(result, 0, EPSILON);

        char key[2] = "!";
        for (size_t i = 0; i < num_keys; i++) {
            HashTable_Insert(sut, key, strlen(key) + 1, &value);
            key[0]++;
        }

        CU_ASSERT_EQUAL(HashTable_GetN(sut), num_keys);

        result = HashTable_GetLoadFactor(sut);
        double expected = (double)num_keys / (double)n;
        CU_ASSERT_DOUBLE_EQUAL(result, expected, EPSILON);
    });
}

static void HashTable_GetN_null_paramter()
{
    ErrorReporter_Clear();
    size_t result = HashTable_GetN(NULL);
    CU_ASSERT_EQUAL(result, ERROR_VAL);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);
}

static void HashTable_Find_null_parameter()
{
    char* key = "42";

    ErrorReporter_Clear();
    void* result = HashTable_Find(NULL, NULL, 10);
    CU_ASSERT_EQUAL(NULL, result);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);

    ErrorReporter_Clear();
    result = HashTable_Find(NULL, key, 10);
    CU_ASSERT_EQUAL(NULL, result);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);

    SUT({
        ErrorReporter_Clear();
        void* result = HashTable_Find(sut, NULL, 10);
        CU_ASSERT_EQUAL(NULL, result);
        CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);
    });
}

static void HashTable_Find_not_found()
{
    char* not_found = "401";
    SUT({
        ErrorReporter_Clear();
        void* result = HashTable_Find(sut, not_found, strlen(not_found) + 1);
        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NotFound);
    });
}

static void HashTable_Find_happy_path()
{
    int one = 1;
    int two = 2;

    char* key_one = "one";
    char* key_two = "two";

    SUT({
        HashTable_Insert(sut, key_one, strlen(key_one) + 1, &one);
        HashTable_Insert(sut, key_two, strlen(key_two) + 1, &two);

        void* result = HashTable_Find(sut, key_one, strlen(key_one) + 1);
        CU_ASSERT_PTR_EQUAL(result, &one);

        result = HashTable_Find(sut, key_two, strlen(key_two) + 1);
        CU_ASSERT_PTR_EQUAL(result, &two);
    });
}

static void HashTable_Destroy_null() { HashTable_Destroy(NULL, NULL); }

static void HashTable_Destroy_frees_items()
{
    int* one = malloc(sizeof(int));
    int* two = malloc(sizeof(int));
    char* key_one = "one";
    char* key_two = "two";

    // failures will get caught by address sanitizer
    HashTable* sut = HashTable_Create(n);
    HashTable_Insert(sut, key_one, strlen(key_one) + 1, one);
    HashTable_Insert(sut, key_two, strlen(key_two) + 1, two);
    HashTable_Destroy(sut, free);
}

static void HashTable_int_keys()
{
    int key = 138;
    char* value = "we are";
    int key2 = 138138;
    char* value2 = "we are, we are";

    SUT({
        Result result = HashTable_Insert(sut, &key, sizeof(int), value);
        CU_ASSERT_EQUAL(result, Success);
        result = HashTable_Insert(sut, &key2, sizeof(int), value2);
        CU_ASSERT_EQUAL(result, Success);

        CU_ASSERT_PTR_EQUAL(HashTable_Find(sut, &key, sizeof(int)), value);
        CU_ASSERT_PTR_EQUAL(HashTable_Find(sut, &key2, sizeof(int)), value2);
    });
}

static void HashTable_long_keys()
{
    long long int key = 138;
    char* value = "we are";
    long long int key2 = -3;
    char* value2 = "we are, we are";

    SUT({
        Result result
            = HashTable_Insert(sut, &key, sizeof(long long int), value);
        CU_ASSERT_EQUAL(result, Success);
        result = HashTable_Insert(sut, &key2, sizeof(long long int), value2);
        CU_ASSERT_EQUAL(result, Success);

        CU_ASSERT_PTR_EQUAL(
            HashTable_Find(sut, &key, sizeof(long long int)), value);
        CU_ASSERT_PTR_EQUAL(
            HashTable_Find(sut, &key2, sizeof(long long int)), value2);
    });
}

static void HashTable_GetCollisions_null_parameter()
{
    ErrorReporter_Clear();
    size_t result = HashTable_GetCollisions(NULL);
    CU_ASSERT_EQUAL(result, ERROR_VAL);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);
}

static void HashTable_GetCollisions_happy_path()
{
    int *one = malloc(sizeof(int)), *two = malloc(sizeof(int)),
        *three = malloc(sizeof(int));
    char *one_key = "one", *two_key = "two", *three_key = "three";
    HashTable* sut = HashTable_Create(2);

    HashTable_Insert(sut, one_key, strlen(one_key) + 1, one);
    HashTable_Insert(sut, two_key, strlen(two_key) + 1, two);
    HashTable_Insert(sut, three_key, strlen(three_key) + 1, three);

    CU_ASSERT_EQUAL(HashTable_GetCollisions(sut), 1);

    HashTable_Destroy(sut, free);
}

int register_hash_table_tests()
{
    CU_TestInfo hash_table_tests[] = { CU_TEST_INFO(
                                           HashTable_Create_failed_malloc),
        CU_TEST_INFO(HashTable_Create_invalid_size),
        CU_TEST_INFO(HashTable_Create_happy_path),
        CU_TEST_INFO(HashTable_Insert_null_parameter),
        CU_TEST_INFO(HashTable_Insert_failed_malloc),
        CU_TEST_INFO(HashTable_Insert_happy_path),
        CU_TEST_INFO(HashTable_Insert_chains),
        CU_TEST_INFO(HashTable_Insert_updates_existing),
        CU_TEST_INFO(HashTable_Delete_not_found),
        CU_TEST_INFO(HashTable_Delete_null_parameter),
        CU_TEST_INFO(HashTable_Delete_happy_path),
        CU_TEST_INFO(HashTable_GetLoadFactor_null_parameter),
        CU_TEST_INFO(HashTable_GetLoadFactor_happy_path),
        CU_TEST_INFO(HashTable_GetN_null_paramter),
        CU_TEST_INFO(HashTable_Find_null_parameter),
        CU_TEST_INFO(HashTable_Find_not_found),
        CU_TEST_INFO(HashTable_Find_happy_path),
        CU_TEST_INFO(HashTable_Destroy_null),
        CU_TEST_INFO(HashTable_Destroy_frees_items),
        CU_TEST_INFO(HashTable_int_keys), CU_TEST_INFO(HashTable_long_keys),
        CU_TEST_INFO(HashTable_GetCollisions_null_parameter),
        CU_TEST_INFO(HashTable_GetCollisions_happy_path), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Hash Table",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = hash_table_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
