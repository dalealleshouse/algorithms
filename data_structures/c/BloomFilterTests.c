#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "BloomFilter.h"
#include "include/ErrorReporter.h"
#include "include/TestHelpers.h"

#define SUT(code_block)                                                        \
    {                                                                          \
        HashTable* sut = HashTable_Create(n);                                  \
        code_block;                                                            \
        HashTable_Destroy(sut, NULL);                                          \
    }

static void BloomFilter_Create_failed_malloc()
{
    FAILED_MALLOC_TEST({
        ErrorReporter_Clear();
        BloomFilter* result = BloomFilter_Create(10);
        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(FailedMemoryAllocation, ErrorReporter_LastErrorCode());
    });
}

int register_bloom_filter_tests()
{
    CU_TestInfo hash_table_tests[]
        = { CU_TEST_INFO(BloomFilter_Create_failed_malloc), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Bloom Filters",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = hash_table_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
