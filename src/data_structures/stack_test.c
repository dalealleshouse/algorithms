/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "stack.h"

#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "malloc_test_wrapper.h"
#include "test_helpers.h"

typedef struct Stack_Item {
  void* payload;
  struct Stack_Item* next;
} Stack_Item;

struct Stack_t {
  size_t n;
  Stack_Item* head;
};

#define SUT(code_block)                          \
  {                                              \
    Stack* sut = NULL;                           \
    ResultCode result_code = Stack_Create(&sut); \
    CU_ASSERT_EQUAL(result_code, kSuccess);      \
                                                 \
    code_block;                                  \
                                                 \
    Stack_Destroy(sut);                          \
  }

/*************************** Stack_Create *************************************/
static void Stack_Create_failed_malloc() {
#if !defined(NDEBUG)
  FAILED_MALLOC_TEST({
    Stack* sut = NULL;
    ResultCode result_code = Stack_Create(&sut);
    CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
    CU_ASSERT_PTR_NULL(sut);
  });
#endif
}

static void Stack_Create_initalizes_values() {
  Stack* sut = NULL;
  ResultCode result_code = Stack_Create(&sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  CU_ASSERT_PTR_NULL(sut->head);
  CU_ASSERT_EQUAL(0, sut->n);

  Stack_Destroy(sut);
}

/*************************** Stack_Push ***************************************/
static void Stack_Push_failed_malloc() {
  Stack* sut = NULL;
  ResultCode result_code = Stack_Create(&sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  FAILED_MALLOC_TEST({
    int dummy = 5;
    ResultCode result = Stack_Push(sut, (void*)&dummy);
    CU_ASSERT_EQUAL(kFailedMemoryAllocation, result);
  });
  Stack_Destroy(sut);
}

static void Stack_Push_null_parameter() {
  SUT({
    const int dummy = 5;
    ResultCode result = Stack_Push(NULL, (void*)&dummy);
    CU_ASSERT_EQUAL(kNullParameter, result);

    result = Stack_Push(sut, NULL);
    CU_ASSERT_EQUAL(kNullParameter, result);
  });
}

static void Stack_Push_first_item() {
  const int dummy = 5;
  SUT({
    ResultCode result = Stack_Push(sut, (void*)&dummy);

    CU_ASSERT_EQUAL(kSuccess, result);
    CU_ASSERT_PTR_EQUAL(&dummy, sut->head->payload);
    CU_ASSERT_EQUAL(1, sut->n);
  });
}

static void Stack_Push_two_items() {
  SUT({
    const int first_in = 5;
    const int second_in = 10;

    ResultCode result = Stack_Push(sut, (void*)&first_in);
    CU_ASSERT_EQUAL(kSuccess, result);

    result = Stack_Push(sut, (void*)&second_in);
    CU_ASSERT_EQUAL(kSuccess, result);

    CU_ASSERT_PTR_EQUAL(&second_in, sut->head->payload);
    CU_ASSERT_PTR_EQUAL(&first_in, sut->head->next->payload);
    CU_ASSERT_EQUAL(2, sut->n);
  });
}

/*************************** Stack_Pop ************************************/
static void Stack_Pop_null_parameter() {
  SUT({
    void* result = NULL;
    ResultCode result_code = Stack_Pop(NULL, &result);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);
  });
}

static void Stack_Pop_empty() {
  SUT({
    void* result = NULL;
    ResultCode result_code = Stack_Pop(sut, &result);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kUnderflow);
  });
}

static void Stack_Pop_last_item() {
  const int first_in = 5;
  SUT({
    Stack_Push(sut, (void*)&first_in);

    void* result = NULL;
    ResultCode result_code = Stack_Pop(sut, &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);

    CU_ASSERT_PTR_EQUAL(&first_in, result);
    CU_ASSERT_PTR_NULL(sut->head);
    CU_ASSERT_EQUAL(0, sut->n);
  });
}

static void Stack_Pop_is_lifo() {
  const size_t n = 5;
  int expected[] = {1, 2, 3, 4, 5};
  SUT({
    for (size_t i = 0; i < n; i++) Stack_Push(sut, &expected[i]);

    CU_ASSERT_EQUAL(n, sut->n);
    for (size_t i = n; i > 0; i--) {
      CU_ASSERT_EQUAL(i, sut->n);

      int* result = NULL;
      ResultCode result_code = Stack_Pop(sut, (void**)&result);
      CU_ASSERT_EQUAL(result_code, kSuccess);

      CU_ASSERT_EQUAL(expected[i - 1], *result);
      CU_ASSERT_EQUAL(i - 1, sut->n);
    }
  });
}

static void Stack_Pop_heap_pointers() {
  TestThingy* first = malloc(sizeof(TestThingy));
  first->id = 0;
  TestThingy* second = malloc(sizeof(TestThingy));
  second->id = 1;
  TestThingy* third = malloc(sizeof(TestThingy));
  third->id = 2;

  SUT({
    Stack_Push(sut, first);
    Stack_Push(sut, second);
    Stack_Push(sut, third);

    for (int i = 2; i >= 0; i--) {
      TestThingy* curr = NULL;
      ResultCode result_code = Stack_Pop(sut, (void**)&curr);
      CU_ASSERT_EQUAL(result_code, kSuccess);

      CU_ASSERT_EQUAL(i, curr->id);
      CU_ASSERT_EQUAL((size_t)i, sut->n);
    }
  });

  free(first);
  free(second);
  free(third);
}

/*************************** Stack_IsEmpty ************************************/
static void Stack_IsEmtpy_null_parameter() {
  bool result = false;
  result = Stack_IsEmpty(NULL);
  CU_ASSERT_TRUE(result);
}

static void Stack_IsEmtpy_empty() {
  SUT({
    bool result = false;
    result = Stack_IsEmpty(sut);
    CU_ASSERT_TRUE(result);
  });
}

static void Stack_IsEmtpy_not_empty() {
  int first_in = 5;
  SUT({
    Stack_Push(sut, &first_in);
    bool result = true;
    result = Stack_IsEmpty(sut);
    CU_ASSERT_FALSE(result);
  });
}

/*************************** Stack_Destroy ************************************/
static void Stack_Destroy_null() {
  // No error means pass
  Stack_Destroy(NULL);
}

int RegisterStackTests() {
  CU_TestInfo Stack_tests[] = {CU_TEST_INFO(Stack_Create_failed_malloc),
                               CU_TEST_INFO(Stack_Create_initalizes_values),
                               CU_TEST_INFO(Stack_Push_failed_malloc),
                               CU_TEST_INFO(Stack_Push_null_parameter),
                               CU_TEST_INFO(Stack_Push_first_item),
                               CU_TEST_INFO(Stack_Push_two_items),
                               CU_TEST_INFO(Stack_Pop_null_parameter),
                               CU_TEST_INFO(Stack_Pop_last_item),
                               CU_TEST_INFO(Stack_Pop_empty),
                               CU_TEST_INFO(Stack_Pop_is_lifo),
                               CU_TEST_INFO(Stack_Pop_heap_pointers),
                               CU_TEST_INFO(Stack_IsEmtpy_empty),
                               CU_TEST_INFO(Stack_IsEmtpy_null_parameter),
                               CU_TEST_INFO(Stack_IsEmtpy_not_empty),
                               CU_TEST_INFO(Stack_Destroy_null),
                               CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Stack",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Stack_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
