#include "./queue.h"

#include <stdlib.h>

#include "../utils/malloc_test_wrapper.h"
#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

/*************************** Queue_Create *************************************/
static void Queue_Create_failed_malloc() {
#if !defined(NDEBUG)
  FAILED_MALLOC_TEST({
    Queue* sut = Queue_Create();
    CU_ASSERT_PTR_NULL(sut);
  });
#endif
}

static void Queue_Create_initalizes_values() {
  Queue* sut = Queue_Create();

  CU_ASSERT_PTR_NULL(sut->head);
  CU_ASSERT_PTR_NULL(sut->tail);
  CU_ASSERT_EQUAL(0, sut->n);

  Queue_Destroy(sut);
}

/*************************** Queue_Enqueue ************************************/
static void Queue_Enqueue_failed_malloc() {
  Queue* sut = Queue_Create();

  const int dummy = 5;
  FAILED_MALLOC_TEST({
    QueueResult result = Queue_Enqueue(sut, (void*)&dummy);
    CU_ASSERT_EQUAL(Queue_FailedMalloc, result);
  })
  Queue_Destroy(sut);
}

static void Queue_Enqueue_null_parameter() {
  Queue* sut = Queue_Create(NULL);

  const int dummy = 5;
  QueueResult result = Queue_Enqueue(NULL, (void*)&dummy);
  CU_ASSERT_EQUAL(Queue_NullParameter, result);

  result = Queue_Enqueue(sut, NULL);
  CU_ASSERT_EQUAL(Queue_NullParameter, result);

  Queue_Destroy(sut);
}

static void Queue_Enqueue_first_item() {
  Queue* sut = Queue_Create(NULL);

  const int dummy = 5;
  QueueResult result = Queue_Enqueue(sut, (void*)&dummy);
  CU_ASSERT_EQUAL(Queue_Success, result);

  CU_ASSERT_PTR_EQUAL(&dummy, sut->head->payload);
  CU_ASSERT_PTR_EQUAL(&dummy, sut->tail->payload);
  CU_ASSERT_EQUAL(1, sut->n);

  Queue_Destroy(sut);
}

static void Queue_Enqueue_two_items() {
  Queue* sut = Queue_Create(NULL);

  const int first_in = 5;
  const int second_in = 10;
  QueueResult result = Queue_Enqueue(sut, (void*)&first_in);
  CU_ASSERT_EQUAL(Queue_Success, result);

  result = Queue_Enqueue(sut, (void*)&second_in);
  CU_ASSERT_EQUAL(Queue_Success, result);

  CU_ASSERT_PTR_EQUAL(&first_in, sut->head->payload);
  CU_ASSERT_PTR_EQUAL(&second_in, sut->tail->payload);
  CU_ASSERT_PTR_EQUAL(&second_in, sut->head->next->payload);
  CU_ASSERT_EQUAL(2, sut->n);

  Queue_Destroy(sut);
}

/*************************** Queue_Dequeue ************************************/
static void Queue_Dequeue_null_parameter() {
  Queue* sut = Queue_Create(NULL);

  void* result = Queue_Dequeue(NULL);
  CU_ASSERT_PTR_NULL(result);

  Queue_Destroy(sut);
}

static void Queue_Dequeue_empty() {
  Queue* sut = Queue_Create(NULL);

  void* result = Queue_Dequeue(sut);
  CU_ASSERT_PTR_NULL(result);

  Queue_Destroy(sut);
}

static void Queue_Dequeue_last_item() {
  Queue* sut = Queue_Create(NULL);

  const int first_in = 5;
  Queue_Enqueue(sut, (void*)&first_in);
  void* result = Queue_Dequeue(sut);

  CU_ASSERT_PTR_EQUAL(&first_in, result);

  CU_ASSERT_PTR_NULL(sut->head);
  CU_ASSERT_PTR_NULL(sut->tail);
  CU_ASSERT_EQUAL(0, sut->n);

  Queue_Destroy(sut);
}

static void Queue_Dequeue_is_fifo() {
  const size_t n = 5;
  int expected[] = {1, 2, 3, 4, 5};
  Queue* sut = Queue_Create(NULL);

  for (size_t i = 0; i < n; i++) Queue_Enqueue(sut, &expected[i]);

  CU_ASSERT_EQUAL(n, sut->n);
  for (size_t i = 0; i < n; i++) {
    CU_ASSERT_EQUAL(expected[i], *(int*)Queue_Dequeue(sut));
    CU_ASSERT_EQUAL(n - i - 1, sut->n);
  }

  Queue_Destroy(sut);
}

static void Queue_Dequeue_heap_pointers() {
  TestThingy* first = malloc(sizeof(TestThingy));
  first->id = 0;
  TestThingy* second = malloc(sizeof(TestThingy));
  second->id = 1;
  TestThingy* third = malloc(sizeof(TestThingy));
  third->id = 2;

  Queue* sut = Queue_Create(NULL);

  Queue_Enqueue(sut, first);
  Queue_Enqueue(sut, second);
  Queue_Enqueue(sut, third);

  for (int i = 0; i < 3; i++) {
    TestThingy* curr = Queue_Dequeue(sut);
    CU_ASSERT_EQUAL(i, curr->id);
    CU_ASSERT_EQUAL(2 - (size_t)i, sut->n);
  }

  Queue_Destroy(sut);
  free(first);
  free(second);
  free(third);
}

/*************************** Queue_IsEmpty ************************************/
static void Queue_IsEmtpy_null_parameter() {
  bool result = Queue_IsEmpty(NULL);
  CU_ASSERT_EQUAL(true, result);
}

static void Queue_IsEmtpy_empty() {
  Queue* sut = Queue_Create(NULL);
  CU_ASSERT_EQUAL(true, Queue_IsEmpty(sut));
  Queue_Destroy(sut);
}

static void Queue_IsEmtpy_not_empty() {
  int first_in = 5;
  Queue* sut = Queue_Create(NULL);
  Queue_Enqueue(sut, &first_in);
  CU_ASSERT_EQUAL(false, Queue_IsEmpty(sut));
  Queue_Destroy(sut);
}

/*************************** Queue_Destroy ************************************/
static void Queue_Destroy_null() {
  // No error means pass
  Queue_Destroy(NULL);
}

int RegisterQueueTests() {
  CU_TestInfo Queue_tests[] = {CU_TEST_INFO(Queue_Create_failed_malloc),
                               CU_TEST_INFO(Queue_Create_initalizes_values),
                               CU_TEST_INFO(Queue_Enqueue_failed_malloc),
                               CU_TEST_INFO(Queue_Enqueue_null_parameter),
                               CU_TEST_INFO(Queue_Enqueue_first_item),
                               CU_TEST_INFO(Queue_Enqueue_two_items),
                               CU_TEST_INFO(Queue_Dequeue_null_parameter),
                               CU_TEST_INFO(Queue_Dequeue_last_item),
                               CU_TEST_INFO(Queue_Dequeue_empty),
                               CU_TEST_INFO(Queue_Dequeue_is_fifo),
                               CU_TEST_INFO(Queue_Dequeue_heap_pointers),
                               CU_TEST_INFO(Queue_IsEmtpy_empty),
                               CU_TEST_INFO(Queue_IsEmtpy_null_parameter),
                               CU_TEST_INFO(Queue_IsEmtpy_not_empty),
                               CU_TEST_INFO(Queue_Destroy_null),
                               CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Queue",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Queue_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
