#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "Stack.h"
#include "include/TestHelpers.h"

/*************************** Stack_Create *************************************/
static void Stack_Create_malloc_tester(void)
{
    Stack* sut = Stack_Create(NULL);
    CU_ASSERT_PTR_NULL(sut);
}

static void Stack_Create_failed_malloc()
{
    TestFailedMalloc(Stack_Create_malloc_tester);
}

static void Stack_Create_initalizes_values()
{
    Stack* sut = Stack_Create();

    CU_ASSERT_PTR_NULL(sut->head);
    CU_ASSERT_EQUAL(0, sut->n);

    Stack_Destroy(sut);
}

/*************************** Stack_Push ***************************************/
Stack* _stack;
static void Stack_Push_malloc_tester(void)
{
    int dummy = 5;
    StackResult result = Stack_Push(_stack, (void*)&dummy);
    CU_ASSERT_EQUAL(Stack_FailedMalloc, result);
}

static void Stack_Push_failed_malloc()
{
    _stack = Stack_Create(NULL);
    TestFailedMalloc(Stack_Push_malloc_tester);
    Stack_Destroy(_stack);
}

static void Stack_Push_null_parameter()
{
    Stack* sut = Stack_Create(NULL);

    const int dummy = 5;
    StackResult result = Stack_Push(NULL, (void*)&dummy);
    CU_ASSERT_EQUAL(Stack_NullParameter, result);

    result = Stack_Push(sut, NULL);
    CU_ASSERT_EQUAL(Stack_NullParameter, result);

    Stack_Destroy(sut);
}

static void Stack_Push_first_item()
{
    const int dummy = 5;
    Stack* sut = Stack_Create();

    StackResult result = Stack_Push(sut, (void*)&dummy);

    CU_ASSERT_EQUAL(Stack_Success, result);
    CU_ASSERT_PTR_EQUAL(&dummy, sut->head->payload);
    CU_ASSERT_EQUAL(1, sut->n);

    Stack_Destroy(sut);
}

static void Stack_Push_two_items()
{
    Stack* sut = Stack_Create(NULL);

    const int first_in = 5;
    const int second_in = 10;

    StackResult result = Stack_Push(sut, (void*)&first_in);
    CU_ASSERT_EQUAL(Stack_Success, result);

    result = Stack_Push(sut, (void*)&second_in);
    CU_ASSERT_EQUAL(Stack_Success, result);

    CU_ASSERT_PTR_EQUAL(&second_in, sut->head->payload);
    CU_ASSERT_PTR_EQUAL(&first_in, sut->head->next->payload);
    CU_ASSERT_EQUAL(2, sut->n);

    Stack_Destroy(sut);
}

/*************************** Stack_Pop ************************************/
static void Stack_Pop_null_parameter()
{
    Stack* sut = Stack_Create(NULL);

    void* result = Stack_Pop(NULL);
    CU_ASSERT_PTR_NULL(result);

    Stack_Destroy(sut);
}

static void Stack_Pop_empty()
{
    Stack* sut = Stack_Create();

    void* result = Stack_Pop(sut);
    CU_ASSERT_PTR_NULL(result);

    Stack_Destroy(sut);
}

static void Stack_Pop_last_item()
{
    const int first_in = 5;
    Stack* sut = Stack_Create(NULL);

    Stack_Push(sut, (void*)&first_in);
    void* result = Stack_Pop(sut);

    CU_ASSERT_PTR_EQUAL(&first_in, result);
    CU_ASSERT_PTR_NULL(sut->head);
    CU_ASSERT_EQUAL(0, sut->n);

    Stack_Destroy(sut);
}

static void Stack_Pop_is_lifo()
{
    const size_t n = 5;
    int expected[] = { 1, 2, 3, 4, 5 };
    Stack* sut = Stack_Create();

    for (size_t i = 0; i < n; i++)
        Stack_Push(sut, &expected[i]);

    CU_ASSERT_EQUAL(n, sut->n);
    for (size_t i = n; i > 0; i--) {
        CU_ASSERT_EQUAL(i, sut->n);
        CU_ASSERT_EQUAL(expected[i - 1], *(int*)Stack_Pop(sut));
        CU_ASSERT_EQUAL(i - 1, sut->n);
    }

    Stack_Destroy(sut);
}

static void Stack_Pop_heap_pointers()
{
    TestThingy* first = malloc(sizeof(TestThingy));
    first->id = 0;
    TestThingy* second = malloc(sizeof(TestThingy));
    second->id = 1;
    TestThingy* third = malloc(sizeof(TestThingy));
    third->id = 2;

    Stack* sut = Stack_Create(NULL);

    Stack_Push(sut, first);
    Stack_Push(sut, second);
    Stack_Push(sut, third);

    for (int i = 2; i >= 0; i--) {
        TestThingy* curr = Stack_Pop(sut);
        CU_ASSERT_EQUAL(i, curr->id);
        CU_ASSERT_EQUAL((size_t)i, sut->n);
    }

    Stack_Destroy(sut);
    free(first);
    free(second);
    free(third);
}

/*************************** Stack_IsEmpty ************************************/
static void Stack_IsEmtpy_null_parameter()
{
    bool result = Stack_IsEmpty(NULL);
    CU_ASSERT_EQUAL(true, result);
}

static void Stack_IsEmtpy_empty()
{
    Stack* sut = Stack_Create(NULL);
    CU_ASSERT_EQUAL(true, Stack_IsEmpty(sut));
    Stack_Destroy(sut);
}

static void Stack_IsEmtpy_not_empty()
{
    int first_in = 5;
    Stack* sut = Stack_Create(NULL);
    Stack_Push(sut, &first_in);
    CU_ASSERT_EQUAL(false, Stack_IsEmpty(sut));
    Stack_Destroy(sut);
}

/*************************** Stack_Destroy ************************************/
static void Stack_Destroy_null()
{
    // No error means pass
    Stack_Destroy(NULL);
}

int register_stack_tests()
{
    CU_TestInfo Stack_tests[] = { CU_TEST_INFO(Stack_Create_failed_malloc),
        CU_TEST_INFO(Stack_Create_initalizes_values),
        CU_TEST_INFO(Stack_Push_failed_malloc),
        CU_TEST_INFO(Stack_Push_null_parameter),
        CU_TEST_INFO(Stack_Push_first_item), CU_TEST_INFO(Stack_Push_two_items),
        CU_TEST_INFO(Stack_Pop_null_parameter),
        CU_TEST_INFO(Stack_Pop_last_item), CU_TEST_INFO(Stack_Pop_empty),
        CU_TEST_INFO(Stack_Pop_is_lifo), CU_TEST_INFO(Stack_Pop_heap_pointers),
        CU_TEST_INFO(Stack_IsEmtpy_empty),
        CU_TEST_INFO(Stack_IsEmtpy_null_parameter),
        CU_TEST_INFO(Stack_IsEmtpy_not_empty), CU_TEST_INFO(Stack_Destroy_null),
        CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Stack",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Stack_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
