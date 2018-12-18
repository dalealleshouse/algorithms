#include <stdint.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "Heap.h"
#include "include/ErrorReporter.h"
#include "include/TestHelpers.h"

#define SUT(size, code_block)                                                  \
    {                                                                          \
        Heap* sut = CreateSut(size);                                           \
                                                                               \
        code_block;                                                            \
                                                                               \
        Heap_Destroy(sut, TestHeapObj_Destroy);                                \
    }

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

static Heap* CreateSut(size_t size)
{
    Heap* sut = Heap_Create(size, TestComparator);
    for (size_t i = 0; i < size; i++)
        Heap_Insert(sut, TestHeapObj_Create(i));

    return sut;
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

static void Heap_Create_arith_overflow()
{
    ErrorReporter_Clear();
    Heap* sut = Heap_Create(SIZE_MAX, TestComparator);
    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(HeapArithmeticOverflow, ErrorReporter_LastErrorCode());
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

static void Heap_Insert_over_size()
{
    Heap* sut = CreateSut(3);

    TestHeapObj* one_more = TestHeapObj_Create(1000);
    HeapResult result = Heap_Insert(sut, one_more);

    CU_ASSERT_EQUAL(HeapOverflow, result);

    TestHeapObj_Destroy(one_more);
    Heap_Destroy(sut, TestHeapObj_Destroy);
}

static void Heap_Extract_null_parameter()
{
    ErrorReporter_Clear();

    void* result = Heap_Extract(NULL);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), HeapNullParameter);
}

static void Heap_Extract_returns_highest_priority()
{
    const size_t size = 11;
    Heap* sut = CreateSut(size);

    for (int i = 0; i < (int)size; i++) {
        TestHeapObj* obj = Heap_Extract(sut);
        CU_ASSERT_EQUAL(i, obj->priority);
        CU_ASSERT_EQUAL(size - i - 1, sut->n);
        HeapIsValid(sut);
        TestHeapObj_Destroy(obj);
    }

    Heap_Destroy(sut, TestHeapObj_Destroy);
}

static void Heap_Extract_empty_heap()
{
    ErrorReporter_Clear();
    Heap* sut = Heap_Create(10, TestComparator);

    void* result = Heap_Extract(sut);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(HeapEmpty, ErrorReporter_LastErrorCode());

    Heap_Destroy(sut, NULL);
}

static void Heap_IsEmpty_return_true_when_empty()
{
    Heap* sut = Heap_Create(10, TestComparator);

    bool result = Heap_IsEmpty(sut);
    CU_ASSERT_TRUE(result);

    Heap_Destroy(sut, NULL);
}

static void Heap_IsEmpty_return_false_when_not_empty()
{
    Heap* sut = CreateSut(1);

    bool result = Heap_IsEmpty(sut);
    CU_ASSERT_FALSE(result);

    Heap_Destroy(sut, TestHeapObj_Destroy);
}

static void Heap_IsEmpty_null_parameter()
{
    ErrorReporter_Clear();

    bool result = Heap_IsEmpty(NULL);
    CU_ASSERT_TRUE(result);
    CU_ASSERT_EQUAL(HeapNullParameter, ErrorReporter_LastErrorCode());
}

static void Heap_Find_null_parameter()
{
    ErrorReporter_Clear();

    void* result = Heap_Find(NULL);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(HeapNullParameter, ErrorReporter_LastErrorCode());
}

static void Heap_Find_returns_highest_priority()
{
    const size_t size = 11;
    Heap* sut = CreateSut(size);

    TestHeapObj* obj = Heap_Find(sut);
    CU_ASSERT_EQUAL(0, obj->priority);
    CU_ASSERT_EQUAL(size, sut->n);
    HeapIsValid(sut);

    Heap_Destroy(sut, TestHeapObj_Destroy);
}

static void Heap_Find_empty()
{
    ErrorReporter_Clear();
    Heap* sut = Heap_Create(10, TestComparator);

    void* result = Heap_Find(sut);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(HeapEmpty, ErrorReporter_LastErrorCode());

    Heap_Destroy(sut, NULL);
}

static void Heap_Resize_null_parameter()
{
    HeapResult result = Heap_Resize(NULL, 5);
    CU_ASSERT_EQUAL(HeapNullParameter, result);
}

static void Heap_Resize_zero()
{
    SUT(10, {
        HeapResult result = Heap_Resize(sut, 0);
        CU_ASSERT_EQUAL(HeapInvalidSize, result);
    });
}

static void Heap_Resize_too_small()
{
    SUT(10, {
        HeapResult result = Heap_Resize(sut, 9);
        CU_ASSERT_EQUAL(HeapInvalidSize, result);
    });
}

static void Heap_Resize_arith_overflow()
{
    SUT(10, {
        ErrorReporter_Clear();
        HeapResult result = Heap_Resize(sut, SIZE_MAX);
        CU_ASSERT_EQUAL(HeapArithmeticOverflow, result);
    });
}

static void Heap_Resize_failed_malloc()
{
    SUT(10, {
        FAILED_MALLOC_TEST({
            HeapResult result = Heap_Resize(sut, 20);
            CU_ASSERT_EQUAL(HeapFailedMemoryAllocation, result);
        });
    });
}

static void Heap_Resize_smaller_happy_path()
{
    SUT(10, {
        void* item = Heap_Extract(sut);
        TestHeapObj_Destroy(item);
        HeapResult result = Heap_Resize(sut, 9);

        CU_ASSERT_EQUAL(HeapSuccess, result);
        CU_ASSERT_EQUAL(9, sut->size);
    });
}

static void Heap_Resize_larger_happy_path()
{
    SUT(10, {
        HeapResult result = Heap_Resize(sut, 20);
        CU_ASSERT_EQUAL(HeapSuccess, result);
        CU_ASSERT_EQUAL(20, sut->size);

        TestHeapObj* obj = TestHeapObj_Create(5);
        result = Heap_Insert(sut, obj);
        CU_ASSERT_EQUAL(HeapSuccess, result);
        CU_ASSERT_EQUAL(11, sut->n);
    });
}

static void Heap_Destroy_null_parameter() { Heap_Destroy(NULL, free); }

int register_heap_tests()
{
    CU_TestInfo Heap_tests[] = { CU_TEST_INFO(Heap_Create_null_parameter),
        CU_TEST_INFO(Heap_Create_invalid_size),
        CU_TEST_INFO(Heap_Create_failed_malloc),
        CU_TEST_INFO(Heap_Create_intializes_variables),
        CU_TEST_INFO(Heap_Create_arith_overflow),
        CU_TEST_INFO(Heap_Insert_null_parameter),
        CU_TEST_INFO(Heap_Insert_first_item),
        CU_TEST_INFO(Heap_Insert_over_size),
        CU_TEST_INFO(Heap_Insert_many_items),
        CU_TEST_INFO(Heap_Destroy_null_parameter),
        CU_TEST_INFO(Heap_Extract_null_parameter),
        CU_TEST_INFO(Heap_Extract_returns_highest_priority),
        CU_TEST_INFO(Heap_IsEmpty_return_true_when_empty),
        CU_TEST_INFO(Heap_IsEmpty_return_false_when_not_empty),
        CU_TEST_INFO(Heap_IsEmpty_null_parameter),
        CU_TEST_INFO(Heap_Find_null_parameter),
        CU_TEST_INFO(Heap_Find_returns_highest_priority),
        CU_TEST_INFO(Heap_Find_empty), CU_TEST_INFO(Heap_Extract_empty_heap),
        CU_TEST_INFO(Heap_Resize_null_parameter),
        CU_TEST_INFO(Heap_Resize_zero), CU_TEST_INFO(Heap_Resize_too_small),
        CU_TEST_INFO(Heap_Resize_arith_overflow),
        CU_TEST_INFO(Heap_Resize_failed_malloc),
        CU_TEST_INFO(Heap_Resize_smaller_happy_path),
        CU_TEST_INFO(Heap_Resize_larger_happy_path), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Heap",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Heap_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
