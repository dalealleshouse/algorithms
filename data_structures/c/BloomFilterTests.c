#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "BloomFilter.h"
#include "HashFunctions.h"
#include "include/ErrorReporter.h"
#include "include/TestHelpers.h"

#define SUT(code_block)                                                        \
    {                                                                          \
        BloomFilter* sut = BloomFilter_Create(bits, funcs);                    \
        code_block;                                                            \
        BloomFilter_Destroy(sut);                                              \
    }

static const size_t bits = 512;
static const size_t funcs = 7;

static void BloomFilter_Create_failed_malloc()
{
    FAILED_MALLOC_TEST({
        ErrorReporter_Clear();
        BloomFilter* result = BloomFilter_Create(bits, funcs);
        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(FailedMemoryAllocation, ErrorReporter_LastErrorCode());
    });
}

static void BloomFilter_Create_invalid_bits()
{
    // Less than 8
    ErrorReporter_Clear();
    BloomFilter* sut = BloomFilter_Create(4, funcs);
    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), ArgumentOutOfRange);

    // Not a power of 2
    ErrorReporter_Clear();
    sut = BloomFilter_Create(1050, funcs);
    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), ArgumentOutOfRange);
}

static void BloomFilter_Create_invalid_funcs()
{
    // 0 hash functions
    ErrorReporter_Clear();
    BloomFilter* sut = BloomFilter_Create(bits, 0);
    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), ArgumentOutOfRange);

    // more than available
    ErrorReporter_Clear();
    sut = BloomFilter_Create(bits, hasher_count + 1);
    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), ArgumentOutOfRange);
}

static void BloomFilter_Create_happy_path()
{
    SUT({ CU_ASSERT_PTR_NOT_NULL(sut); });
}

static void BloomFilter_Insert_null_parameter()
{
    char* key = "life is pain";

    Result result = BloomFilter_Insert(NULL, key);
    CU_ASSERT_EQUAL(NullParameter, result);

    SUT({
        result = BloomFilter_Insert(sut, NULL);
        CU_ASSERT_EQUAL(NullParameter, result);
    });
}

static void BloomFilter_Insert_happy_path()
{
    char* key = "life is love";

    SUT({
        Result result = BloomFilter_Insert(sut, key);
        CU_ASSERT_EQUAL(Success, result);
    });
}

static void BloomFilter_Lookup_null_parameter()
{
    char* key = "life is beauty";

    ErrorReporter_Clear();
    Result result = BloomFilter_Lookup(NULL, key);
    CU_ASSERT_FALSE(result);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);

    SUT({
        ErrorReporter_Clear();
        result = BloomFilter_Lookup(sut, NULL);
        CU_ASSERT_FALSE(result);
        CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);
    });
}

static void BloomFilter_Lookup_not_found()
{
    SUT({
        bool result = BloomFilter_Lookup(sut, "not found");
        CU_ASSERT_FALSE(result);
    });
}

static void BloomFilter_Lookup_found()
{
    char* key = "life is an illusion";
    SUT({
        BloomFilter_Insert(sut, (void*)key);
        bool result = BloomFilter_Lookup(sut, (void*)key);
        CU_ASSERT_TRUE(result);
    });
}

static void BloomFilter_Lookup_happy_path()
{
    const size_t n = 10;
    char* exists[] = { "life", "is", "pain", "love", "an illusion", "a quest",
        "beauty", "shit", "complicated", "code" };
    char* not_found[] = { "401", "not found", "vacant", "empty", "blank",
        "barren", "hollow", "desolate", "void", "deserted" };

    SUT({
        for (size_t i = 0; i < n; i++)
            BloomFilter_Insert(sut, exists[i]);

        for (size_t i = 0; i < n; i++) {
            CU_ASSERT_TRUE(BloomFilter_Lookup(sut, exists[i]));
            CU_ASSERT_FALSE(BloomFilter_Lookup(sut, not_found[i]));
        }
    });
}

static void BloomFilter_GetN_null_parameter()
{
    ErrorReporter_Clear();
    size_t result = BloomFilter_GetN(NULL);
    CU_ASSERT_EQUAL(result, N_ERROR);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), NullParameter);
}

static void BloomFilter_GetN_happy_path()
{
    SUT({
        CU_ASSERT_EQUAL(BloomFilter_GetN(sut), 0);
        BloomFilter_Insert(sut, "random key");
        CU_ASSERT_EQUAL(BloomFilter_GetN(sut), 1);
        BloomFilter_Insert(sut, "another random key");
        CU_ASSERT_EQUAL(BloomFilter_GetN(sut), 2);
    });
}

int register_bloom_filter_tests()
{
    CU_TestInfo hash_table_tests[]
        = { CU_TEST_INFO(BloomFilter_Create_failed_malloc),
              CU_TEST_INFO(BloomFilter_Create_invalid_bits),
              CU_TEST_INFO(BloomFilter_Create_happy_path),
              CU_TEST_INFO(BloomFilter_Create_invalid_funcs),
              CU_TEST_INFO(BloomFilter_Insert_null_parameter),
              CU_TEST_INFO(BloomFilter_Insert_happy_path),
              CU_TEST_INFO(BloomFilter_Lookup_null_parameter),
              CU_TEST_INFO(BloomFilter_Lookup_not_found),
              CU_TEST_INFO(BloomFilter_Lookup_found),
              CU_TEST_INFO(BloomFilter_Lookup_happy_path),
              CU_TEST_INFO(BloomFilter_GetN_null_parameter),
              CU_TEST_INFO(BloomFilter_GetN_happy_path), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Bloom Filters",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = hash_table_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
