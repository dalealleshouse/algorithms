#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "MemAllocMock.h"
#include "TestHelpers.h"

void TestFaliedMalloc(void (*tester)(void))
{
    MemAllocMock_InterceptMalloc(NULL);
    tester();
    CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());
    MemAllocMock_ResetMalloc();
}
