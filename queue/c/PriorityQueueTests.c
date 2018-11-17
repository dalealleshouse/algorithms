#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "PriorityQueue.h"
#include "include/ErrorReporter.h"
#include "include/TestHelpers.h"

static void test_obj_destroy(void* t) { free(t); }

static TestThingy* test_obj_init(int priority)
{
    TestThingy* t = calloc(sizeof(TestThingy), 1);
    t->x = priority;
    return t;
}

// ASSUMPTION: Null are never sent here
// Prioritizes lower values
static int test_comparator(const void* x, const void* y)
{
    TestThingy* t1 = (TestThingy*)x;
    TestThingy* t2 = (TestThingy*)y;

    if (t1->x == t2->x)
        return 0;

    if (t1->x < t2->x)
        return 1;

    return -1;
}

static void PQ_Create_malloc_tester(void)
{
    PriorityQueue* sut = PQ_Create(test_comparator);
    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(PQ_FailedMalloc, ErrorReporter_LastErrorCode());
}

static void PQ_Create_failed_malloc()
{
    TestFailedMalloc(PQ_Create_malloc_tester);
}

static void PQ_Create_initalizes_values()
{
    PriorityQueue* sut = PQ_Create(test_comparator);

    CU_ASSERT_PTR_NULL(sut->head);
    CU_ASSERT_PTR_NULL(sut->tail);
    CU_ASSERT_EQUAL(0, sut->n);
    CU_ASSERT_EQUAL(test_comparator, sut->comparator);

    PQ_Destroy(sut, NULL);
}

static void PQ_Create_null_parameter()
{
    PriorityQueue* sut = PQ_Create(NULL);
    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(PQ_NullParameter, ErrorReporter_LastErrorCode());
    PQ_Destroy(sut, NULL);
}

PriorityQueue* _sut;
static void PQ_Insert_malloc_tester(void)
{
    TestThingy t;

    PQResult result = PQ_Insert(_sut, &t);
    CU_ASSERT_EQUAL(PQ_FailedMalloc, result);
}

static void PQ_Insert_failed_malloc()
{
    _sut = PQ_Create(test_comparator);
    TestFailedMalloc(PQ_Insert_malloc_tester);
    PQ_Destroy(_sut, NULL);
}

static void PQ_Insert_null_parameter()
{
    TestThingy t;
    PriorityQueue* sut = PQ_Create(test_comparator);

    PQResult result = PQ_Insert(NULL, &t);
    CU_ASSERT_EQUAL(PQ_NullParameter, result);

    result = PQ_Insert(sut, NULL);
    CU_ASSERT_EQUAL(PQ_NullParameter, result);

    PQ_Destroy(sut, NULL);
}

static void PQ_Insert_first_item()
{
    TestThingy* t = test_obj_init(1);
    PriorityQueue* sut = PQ_Create(test_comparator);

    PQResult result = PQ_Insert(sut, t);
    CU_ASSERT_EQUAL(PQ_Success, result);

    CU_ASSERT_PTR_EQUAL(t, sut->head->payload);
    CU_ASSERT_PTR_EQUAL(t, sut->tail->payload);
    CU_ASSERT_EQUAL(1, sut->n);

    PQ_Destroy(sut, test_obj_destroy);
}

static void PQ_Insert_two_items()
{
    TestThingy* t1 = test_obj_init(1);
    TestThingy* t2 = test_obj_init(2);
    PriorityQueue* sut = PQ_Create(test_comparator);

    PQResult result = PQ_Insert(sut, t2);
    CU_ASSERT_EQUAL(PQ_Success, result);

    result = PQ_Insert(sut, t1);
    CU_ASSERT_EQUAL(PQ_Success, result);

    CU_ASSERT_PTR_EQUAL(t1, sut->head->payload);
    CU_ASSERT_PTR_EQUAL(t2, sut->tail->payload);
    CU_ASSERT_PTR_EQUAL(t2, sut->head->next->payload);
    CU_ASSERT_EQUAL(2, sut->n);

    PQ_Destroy(sut, test_obj_destroy);
}

static void PQ_Remove_null_parameter()
{
    ErrorReporter_Clear();
    PriorityQueue* sut = PQ_Create(test_comparator);

    void* result = PQ_Remove(NULL);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(PQ_NullParameter, ErrorReporter_LastErrorCode());

    PQ_Destroy(sut, NULL);
}

static void PQ_Remove_empty()
{
    ErrorReporter_Clear();
    PriorityQueue* sut = PQ_Create(test_comparator);

    void* result = PQ_Remove(sut);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(PQ_EmptyQueue, ErrorReporter_LastErrorCode());

    PQ_Destroy(sut, NULL);
}

static void PQ_Remove_last_item()
{
    TestThingy* t = test_obj_init(1);
    PriorityQueue* sut = PQ_Create(test_comparator);

    PQ_Insert(sut, t);
    void* result = PQ_Remove(sut);

    CU_ASSERT_PTR_EQUAL(t, result);

    CU_ASSERT_PTR_NULL(sut->head);
    CU_ASSERT_PTR_NULL(sut->tail);
    CU_ASSERT_EQUAL(0, sut->n);

    PQ_Destroy(sut, test_obj_destroy);
    test_obj_destroy(t);
}

static void PQ_Remove_priority_order()
{
    const size_t n = 5;

    TestThingy* expected[n];
    for (size_t i = 0; i < n; i++)
        expected[i] = test_obj_init(i);

    PriorityQueue* sut = PQ_Create(test_comparator);

    PQ_Insert(sut, expected[2]);
    PQ_Insert(sut, expected[4]);
    PQ_Insert(sut, expected[0]);
    PQ_Insert(sut, expected[3]);
    PQ_Insert(sut, expected[1]);

    CU_ASSERT_EQUAL(n, sut->n);
    CU_ASSERT_PTR_EQUAL(expected[0], sut->head->payload);
    CU_ASSERT_PTR_EQUAL(expected[4], sut->tail->payload);

    for (size_t i = 0; i < n; i++)
        CU_ASSERT_EQUAL(expected[i], PQ_Remove(sut));

    CU_ASSERT_PTR_NULL(sut->head);
    CU_ASSERT_PTR_NULL(sut->tail);
    CU_ASSERT_EQUAL(0, sut->n);

    PQ_Destroy(sut, NULL);
    for (size_t i = 0; i < n; i++)
        test_obj_destroy(expected[i]);
}

static void PQ_IsEmpty_null_parameter()
{
    ErrorReporter_Clear();
    bool result = PQ_IsEmpty(NULL);
    CU_ASSERT_TRUE(result);
    CU_ASSERT_EQUAL(PQ_NullParameter, ErrorReporter_LastErrorCode());
}

static void PQ_IsEmpty_empty()
{
    PriorityQueue* sut = PQ_Create(NULL);
    CU_ASSERT_TRUE(PQ_IsEmpty(sut));
    PQ_Destroy(sut, NULL);
}

static void PQ_IsEmpty_not_empty()
{
    TestThingy* t = test_obj_init(1);
    PriorityQueue* sut = PQ_Create(test_comparator);
    PQ_Insert(sut, t);
    CU_ASSERT_FALSE(PQ_IsEmpty(sut));
    PQ_Destroy(sut, test_obj_destroy);
}

static void PQ_Peek_null_parameter()
{
    ErrorReporter_Clear();
    PriorityQueue* sut = PQ_Create(test_comparator);

    void* result = PQ_Peek(NULL);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(PQ_NullParameter, ErrorReporter_LastErrorCode());

    PQ_Destroy(sut, NULL);
}

static void PQ_Peek_empty()
{
    ErrorReporter_Clear();
    PriorityQueue* sut = PQ_Create(test_comparator);

    void* result = PQ_Peek(sut);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(PQ_EmptyQueue, ErrorReporter_LastErrorCode());

    PQ_Destroy(sut, NULL);
}

static void PQ_Peek_not_empty()
{
    TestThingy* t = test_obj_init(1);
    PriorityQueue* sut = PQ_Create(test_comparator);
    PQ_Insert(sut, t);

    void* result = PQ_Peek(sut);

    CU_ASSERT_PTR_EQUAL(t, result);
    CU_ASSERT_EQUAL(1, sut->n);

    PQ_Destroy(sut, test_obj_destroy);
}

static void PQ_Destroy_null()
{
    // No error means pass
    PQ_Destroy(NULL, NULL);
}

int register_priority_queue_tests()
{
    CU_TestInfo Queue_tests[] = { CU_TEST_INFO(PQ_Create_failed_malloc),
        CU_TEST_INFO(PQ_Create_initalizes_values),
        CU_TEST_INFO(PQ_Create_null_parameter),
        CU_TEST_INFO(PQ_Insert_failed_malloc),
        CU_TEST_INFO(PQ_Insert_null_parameter),
        CU_TEST_INFO(PQ_Insert_first_item), CU_TEST_INFO(PQ_Insert_two_items),
        CU_TEST_INFO(PQ_Remove_null_parameter), CU_TEST_INFO(PQ_Remove_empty),
        CU_TEST_INFO(PQ_Remove_last_item),
        CU_TEST_INFO(PQ_Remove_priority_order),
        CU_TEST_INFO(PQ_IsEmpty_null_parameter), CU_TEST_INFO(PQ_IsEmpty_empty),
        CU_TEST_INFO(PQ_IsEmpty_not_empty), CU_TEST_INFO(PQ_Destroy_null),
        CU_TEST_INFO(PQ_Peek_null_parameter), CU_TEST_INFO(PQ_Peek_empty),
        CU_TEST_INFO(PQ_Peek_not_empty), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Priority Queue",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Queue_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
