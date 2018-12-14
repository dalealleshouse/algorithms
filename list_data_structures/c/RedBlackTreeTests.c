#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "RedBlackTree.h"
#include "include/MemAllocMock.h"
#include "include/TestHelpers.h"

static void RedBlackTree_Stub() { CU_FAIL(); }

int register_red_black_tests()
{
    CU_TestInfo stub_tests[]
        = { CU_TEST_INFO(RedBlackTree_Stub), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "RedBlackTree_Stub",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = stub_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
