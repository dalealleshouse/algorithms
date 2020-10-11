/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "./queue.h"

#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "malloc_test_wrapper.h"
#include "test_helpers.h"

#define SUT(code_block)                          \
  {                                              \
    Queue* sut = NULL;                           \
    ResultCode result_code = Queue_Create(&sut); \
    CU_ASSERT_EQUAL(result_code, kSuccess);      \
                                                 \
    code_block;                                  \
                                                 \
    Queue_Destroy(sut);                          \
  }

typedef struct Queue_Item {
  void* payload;
  struct Queue_Item* next;
} Queue_Item;

struct Queue_t {
  size_t n;
  Queue_Item* head;
  Queue_Item* tail;
};

/*************************** Queue_Create *************************************/
static void Queue_Create_failed_malloc() {
#if !defined(NDEBUG)
  FAILED_MALLOC_TEST({
    Queue* sut = NULL;
    ResultCode result_code = Queue_Create(&sut);
    CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
    CU_ASSERT_PTR_NULL(sut);
  });
#endif
}

static void Queue_Create_initalizes_values() {
  SUT({
    CU_ASSERT_PTR_NULL(sut->head);
    CU_ASSERT_PTR_NULL(sut->tail);
    CU_ASSERT_EQUAL(0, sut->n);
  });
}

/*************************** Queue_Enqueue ************************************/
static void Queue_Enqueue_failed_malloc() {
  SUT({
    const int dummy = 5;
    FAILED_MALLOC_TEST({
      ResultCode result = Queue_Enqueue(sut, (void*)&dummy);
      CU_ASSERT_EQUAL(kFailedMemoryAllocation, result);
    })
  });
}

static void Queue_Enqueue_null_parameter() {
  SUT({
    const int dummy = 5;
    ResultCode result = Queue_Enqueue(NULL, (void*)&dummy);
    CU_ASSERT_EQUAL(kNullParameter, result);

    result = Queue_Enqueue(sut, NULL);
    CU_ASSERT_EQUAL(kNullParameter, result);
  });
}

static void Queue_Enqueue_first_item() {
  SUT({
    const int dummy = 5;
    ResultCode result = Queue_Enqueue(sut, (void*)&dummy);
    CU_ASSERT_EQUAL(kSuccess, result);

    CU_ASSERT_PTR_EQUAL(&dummy, sut->head->payload);
    CU_ASSERT_PTR_EQUAL(&dummy, sut->tail->payload);
    CU_ASSERT_EQUAL(1, sut->n);
  });
}

static void Queue_Enqueue_two_items() {
  SUT({
    const int first_in = 5;
    const int second_in = 10;
    ResultCode result = Queue_Enqueue(sut, (void*)&first_in);
    CU_ASSERT_EQUAL(kSuccess, result);

    result = Queue_Enqueue(sut, (void*)&second_in);
    CU_ASSERT_EQUAL(kSuccess, result);

    CU_ASSERT_PTR_EQUAL(&first_in, sut->head->payload);
    CU_ASSERT_PTR_EQUAL(&second_in, sut->tail->payload);
    CU_ASSERT_PTR_EQUAL(&second_in, sut->head->next->payload);
    CU_ASSERT_EQUAL(2, sut->n);
  });
}

/*************************** Queue_Dequeue ************************************/
static void Queue_Dequeue_null_parameter() {
  SUT({
    ResultCode result_code = Queue_Dequeue(sut, NULL);
    CU_ASSERT_EQUAL(kNullParameter, result_code);
  });
}

static void Queue_Dequeue_empty() {
  SUT({
    void* result = NULL;
    ResultCode result_code = Queue_Dequeue(sut, &result);
    CU_ASSERT_EQUAL(kEmpty, result_code);
    CU_ASSERT_PTR_NULL(result);
  });
}

static void Queue_Dequeue_last_item() {
  SUT({
    const int first_in = 5;
    Queue_Enqueue(sut, (void*)&first_in);
    void* result = NULL;
    ResultCode result_code = Queue_Dequeue(sut, &result);

    CU_ASSERT_EQUAL(kSuccess, result_code);
    CU_ASSERT_PTR_EQUAL(&first_in, result);

    CU_ASSERT_PTR_NULL(sut->head);
    CU_ASSERT_PTR_NULL(sut->tail);
    CU_ASSERT_EQUAL(0, sut->n);
  });
}

static void Queue_Dequeue_is_fifo() {
  const size_t n = 5;
  int expected[] = {1, 2, 3, 4, 5};
  SUT({
    for (size_t i = 0; i < n; i++) {
      ResultCode result_code = Queue_Enqueue(sut, &expected[i]);
      CU_ASSERT_EQUAL(kSuccess, result_code);
    }

    CU_ASSERT_EQUAL(n, sut->n);
    for (size_t i = 0; i < n; i++) {
      int* result = NULL;
      ResultCode result_code = Queue_Dequeue(sut, (void**)&result);
      CU_ASSERT_EQUAL(kSuccess, result_code);
      CU_ASSERT_EQUAL(expected[i], *result);
      CU_ASSERT_EQUAL(n - i - 1, sut->n);
    }
  });
}

static void Queue_Dequeue_heap_pointers() {
  TestThingy* first = malloc(sizeof(TestThingy));
  first->id = 0;
  TestThingy* second = malloc(sizeof(TestThingy));
  second->id = 1;
  TestThingy* third = malloc(sizeof(TestThingy));
  third->id = 2;

  SUT({
    Queue_Enqueue(sut, first);
    Queue_Enqueue(sut, second);
    Queue_Enqueue(sut, third);

    for (int i = 0; i < 3; i++) {
      TestThingy* curr = NULL;
      Queue_Dequeue(sut, (void**)&curr);
      CU_ASSERT_EQUAL(i, curr->id);
      CU_ASSERT_EQUAL(2 - (size_t)i, sut->n);
    }
  });

  free(first);
  free(second);
  free(third);
}

/************************** Queue_IskEmpty ************************************/
static void Queue_IsEmtpy_null_parameter() {
  bool result = Queue_IsEmpty(NULL);
  CU_ASSERT_EQUAL(true, result);
}

static void Queue_IsEmtpy_empty() {
  SUT({ CU_ASSERT_EQUAL(true, Queue_IsEmpty(sut)); });
}

static void Queue_IsEmtpy_not_empty() {
  int first_in = 5;
  SUT({
    Queue_Enqueue(sut, &first_in);
    CU_ASSERT_EQUAL(false, Queue_IsEmpty(sut));
  });
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
