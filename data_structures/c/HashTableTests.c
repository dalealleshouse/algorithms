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
        HashTable* result = HashTable_Create(10);
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

    Result result = HashTable_Insert(NULL, NULL, NULL);
    CU_ASSERT_EQUAL(NullParameter, result);

    result = HashTable_Insert(NULL, key, &value);
    CU_ASSERT_EQUAL(NullParameter, result);

    SUT({
        result = HashTable_Insert(sut, NULL, &value);
        CU_ASSERT_EQUAL(NullParameter, result);

        result = HashTable_Insert(sut, key, NULL);
        CU_ASSERT_EQUAL(NullParameter, result);
    });
}

static void HashTable_Insert_failed_malloc()
{
    char* key = "we are";
    int value = 138;

    SUT({
        FAILED_MALLOC_TEST({
            Result result = HashTable_Insert(sut, key, &value);
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
        Result result = HashTable_Insert(sut, key, &value);
        CU_ASSERT_EQUAL(Success, result);
        CU_ASSERT_EQUAL(1, HashTable_GetN(sut));

        void* found = HashTable_Find(sut, key);
        CU_ASSERT_PTR_EQUAL(found, &value);
    });
}

static void HashTable_Delete_null_parameter()
{
    char* key = "we are";

    Result result = HashTable_Delete(NULL, NULL);
    CU_ASSERT_EQUAL(NullParameter, result);

    result = HashTable_Delete(NULL, key);
    CU_ASSERT_EQUAL(NullParameter, result);

    SUT({
        result = HashTable_Delete(sut, NULL);
        CU_ASSERT_EQUAL(NullParameter, result);
    });
}

static void HashTable_Delete_not_found()
{
    int value = 5;

    SUT({
        HashTable_Insert(sut, "some key", &value);
        HashTable_Insert(sut, "some other key", &value);
        HashTable_Insert(sut, "yet another", &value);
        CU_ASSERT_EQUAL(HashTable_GetN(sut), 3);

        Result result = HashTable_Delete(sut, "401 not found");
        CU_ASSERT_EQUAL(result, NotFound);
        CU_ASSERT_EQUAL(HashTable_GetN(sut), 3);
    });
}

static void HashTable_Delete_happy_path()
{
    char* key = "we are";
    int value = 138;

    SUT({
        HashTable_Insert(sut, "some key", &value);
        HashTable_Insert(sut, "some other key", &value);
        HashTable_Insert(sut, "yet another", &value);
        HashTable_Insert(sut, key, &value);
        CU_ASSERT_EQUAL(HashTable_GetN(sut), 4);

        Result result = HashTable_Delete(sut, key);
        CU_ASSERT_EQUAL(result, Success);
        CU_ASSERT_EQUAL(HashTable_GetN(sut), 3);
        void* found = HashTable_Find(sut, key);
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
            HashTable_Insert(sut, key, &value);
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
    void* result = HashTable_Find(NULL, NULL);
    CU_ASSERT_EQUAL(NULL, result);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);

    ErrorReporter_Clear();
    result = HashTable_Find(NULL, key);
    CU_ASSERT_EQUAL(NULL, result);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);

    SUT({
        ErrorReporter_Clear();
        void* result = HashTable_Find(sut, NULL);
        CU_ASSERT_EQUAL(NULL, result);
        CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);
    });
}

static void HashTable_Find_not_found()
{
    char* not_found = "401";
    SUT({
        ErrorReporter_Clear();
        void* result = HashTable_Find(sut, not_found);
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
        HashTable_Insert(sut, key_one, &one);
        HashTable_Insert(sut, key_two, &two);

        void* result = HashTable_Find(sut, key_one);
        CU_ASSERT_PTR_EQUAL(result, &one);

        result = HashTable_Find(sut, key_two);
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
    HashTable_Insert(sut, key_one, one);
    HashTable_Insert(sut, key_two, two);
    HashTable_Destroy(sut, free);
}

int register_hash_table_tests()
{
    CU_TestInfo hash_table_tests[]
        = { CU_TEST_INFO(HashTable_Create_failed_malloc),
              CU_TEST_INFO(HashTable_Create_invalid_size),
              CU_TEST_INFO(HashTable_Create_happy_path),
              CU_TEST_INFO(HashTable_Insert_null_parameter),
              CU_TEST_INFO(HashTable_Insert_failed_malloc),
              CU_TEST_INFO(HashTable_Insert_happy_path),
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
              CU_TEST_INFO(HashTable_Destroy_frees_items), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Hash Table",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = hash_table_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}