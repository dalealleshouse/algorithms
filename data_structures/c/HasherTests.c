#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "Hasher.h"
#include "include/ErrorReporter.h"
#include "include/TestHelpers.h"

static void compress_hash_standard()
{
    // example from Introduction to Algorithms p. 264
    size_t result = compress_hash(123456, 16384);
    CU_ASSERT_EQUAL(67, result);

    result = compress_hash(15, 8);
    CU_ASSERT_EQUAL(2, result);

    result = compress_hash(23, 8);
    CU_ASSERT_EQUAL(1, result);

    result = compress_hash(100, 8);
    CU_ASSERT_EQUAL(6, result);
}

static void generate_index_standard()
{
    const size_t m = 1000;
    const char* key_one = "similar string";
    const char* key_two = "similar sting";

    size_t result = generate_index(key_one, strlen(key_one), m);
    size_t result2 = generate_index(key_two, strlen(key_two), m);

    CU_ASSERT_NOT_EQUAL(result, result2);
}

int register_hasher_tests()
{
    CU_TestInfo hasher_tests[] = { CU_TEST_INFO(compress_hash_standard),
        CU_TEST_INFO(generate_index_standard), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Hasher",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = hasher_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
