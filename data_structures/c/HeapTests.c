#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "Heap.h"
#include "include/ErrorReporter.h"
#include "include/TestHelpers.h"

typedef struct TestHeapObj {
    int id;
    int priority;
} TestHeapObj;

static TestHeapObj* TestHeapObj_Create(int priority)
{
    TestHeapObj* obj = malloc(sizeof(TestHeapObj));
    obj->id = priority;
    obj->priority = priority;

    return obj;
}

static void TestHeapObj_Destroy(void* obj) { free(obj); }

static void HeapIsValid(Heap* sut)
{
    CU_ASSERT_PTR_NOT_NULL(sut);

    for (size_t i = 1; i < sut->n; i++) {
        size_t parent_index = (i - 1) >> 1;

        TestHeapObj* item = sut->data[i];
        TestHeapObj* parent = sut->data[parent_index];

        if (parent->priority > item->priority) {
            printf("parent=%zu-%d, child=%zu-%d\n", parent_index,
                parent->priority, i, item->priority);
            CU_FAIL("Invalid Heap Value");
        }
    }
}

static int TestComparator(const void* x, const void* y)
{
    TestHeapObj* _x = (TestHeapObj*)x;
    TestHeapObj* _y = (TestHeapObj*)y;

    if (_x->priority == _y->priority)
        return 0;

    if (_x->priority < _y->priority)
        return 1;

    return -1;
}

static void Heap_Create_null_parameter()
{
    ErrorReporter_Clear();
    Heap* sut = Heap_Create(5, NULL);

    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), HeapNullParameter);
}

static void Heap_Create_invalid_size()
{
    ErrorReporter_Clear();
    Heap* sut = Heap_Create(0, TestComparator);

    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), HeapInvalidSize);
}

static void Heap_Create_malloc_tester(void)
{
    Heap* sut = Heap_Create(5, TestComparator);

    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), HeapFailedMemoryAllocation);
}

static void Heap_Create_failed_malloc()
{
    TestFailedMalloc(Heap_Create_malloc_tester);
}

static void Heap_Create_intializes_variables()
{
    const size_t size = 10;
    Heap* sut = Heap_Create(size, TestComparator);

    CU_ASSERT_PTR_NOT_NULL(sut);
    CU_ASSERT_EQUAL(0, sut->n);
    CU_ASSERT_EQUAL(size, sut->size);
    CU_ASSERT_PTR_EQUAL(TestComparator, sut->comparator);

    Heap_Destroy(sut, NULL);
}

static void Heap_Insert_null_parameter()
{
    const size_t size = 10;
    TestHeapObj obj = { 1, 1 };
    Heap* sut = Heap_Create(size, TestComparator);

    HeapResult result = Heap_Insert(sut, NULL);
    CU_ASSERT_EQUAL(HeapNullParameter, result);

    result = Heap_Insert(NULL, &obj);
    CU_ASSERT_EQUAL(HeapNullParameter, result);

    result = Heap_Insert(NULL, NULL);
    CU_ASSERT_EQUAL(HeapNullParameter, result);

    Heap_Destroy(sut, NULL);
}

static void Heap_Insert_first_item()
{
    const size_t size = 10;
    TestHeapObj obj = { 1, 1 };
    Heap* sut = Heap_Create(size, TestComparator);

    HeapResult result = Heap_Insert(sut, &obj);

    CU_ASSERT_EQUAL(HeapSuccess, result);
    CU_ASSERT_PTR_EQUAL(&obj, sut->data[0]);
    CU_ASSERT_EQUAL(1, sut->n);

    Heap_Destroy(sut, NULL);
}

static void Heap_Insert_many_items()
{
    const size_t size = 11;

    Heap* sut = Heap_Create(size, TestComparator);

    for (size_t i = size; i > 0; i--)
        Heap_Insert(sut, TestHeapObj_Create(i));

    CU_ASSERT_EQUAL(size, sut->n);
    HeapIsValid(sut);
    Heap_Destroy(sut, TestHeapObj_Destroy);
}

static void Heap_Insert_over_size() { CU_FAIL("not implemented"); }

int register_heap_tests()
{
    CU_TestInfo Heap_tests[] = { CU_TEST_INFO(Heap_Create_null_parameter),
        CU_TEST_INFO(Heap_Create_invalid_size),
        CU_TEST_INFO(Heap_Create_failed_malloc),
        CU_TEST_INFO(Heap_Create_intializes_variables),
        CU_TEST_INFO(Heap_Insert_null_parameter),
        CU_TEST_INFO(Heap_Insert_first_item),
        CU_TEST_INFO(Heap_Insert_over_size),
        CU_TEST_INFO(Heap_Insert_many_items), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Heap",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Heap_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
