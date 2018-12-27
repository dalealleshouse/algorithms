#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "HashTable.h"
#include "include/ErrorReporter.h"
#include "include/TestHelpers.h"

static void HashTable_Create_null_parameter() { CU_FAIL(); }

int register_hash_table_tests()
{
    CU_TestInfo hash_table_tests[]
        = { CU_TEST_INFO(HashTable_Create_null_parameter), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Hash Table",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = hash_table_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
