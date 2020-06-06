#include "./priority_queue.h"

#include <stdlib.h>

#include "../utils/malloc_test_wrapper.h"
#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

const static size_t _n = 5;
static TestThingy* _items[5];

static void test_obj_destroy(void* t) { free(t); }

static TestThingy* test_obj_init(int priority) {
  TestThingy* t = calloc(sizeof(TestThingy), 1);
  t->x = priority;
  return t;
}

// ASSUMPTION: Null are never sent here
// Prioritizes lower values
static int test_comparator(const void* x, const void* y) {
  TestThingy* t1 = (TestThingy*)x;
  TestThingy* t2 = (TestThingy*)y;

  if (t1->x == t2->x) return 0;

  if (t1->x < t2->x) return 1;

  return -1;
}

static int GetX(PQ_Item* i) {
  if (i == NULL) return -1;

  TestThingy* t = i->payload;
  return t->x;
}

static PriorityQueue* CreateSut() {
  PriorityQueue* sut = PQ_Create(test_comparator);

  for (size_t i = 0; i < _n; i++) {
    _items[i] = test_obj_init(i + 1);
    PQ_Insert(sut, (void*)_items[i]);
  }

  return sut;
}

static void PQ_Create_failed_malloc() {
#if !defined(NDEBUG)
  FAILED_MALLOC_TEST({
    PriorityQueue* sut = PQ_Create(test_comparator);
    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(PQ_FailedMalloc, ErrorReporter_LastErrorCode());
  });
#endif
}

static void PQ_Create_initalizes_values() {
  PriorityQueue* sut = PQ_Create(test_comparator);

  CU_ASSERT_PTR_NULL(sut->head);
  CU_ASSERT_PTR_NULL(sut->tail);
  CU_ASSERT_EQUAL(0, sut->n);
  CU_ASSERT_EQUAL(test_comparator, sut->comparator);

  PQ_Destroy(sut, NULL);
}

static void PQ_Create_null_parameter() {
  PriorityQueue* sut = PQ_Create(NULL);
  CU_ASSERT_PTR_NULL(sut);
  CU_ASSERT_EQUAL(PQ_NullParameter, ErrorReporter_LastErrorCode());
  PQ_Destroy(sut, NULL);
}

PriorityQueue* _sut;
static void PQ_Insert_failed_malloc() {
#if !defined(NDEBUG)
  _sut = PQ_Create(test_comparator);
  FAILED_MALLOC_TEST({
    TestThingy t;

    PQResult result = PQ_Insert(_sut, &t);
    CU_ASSERT_EQUAL(PQ_FailedMalloc, result);
  });
  PQ_Destroy(_sut, NULL);
#endif
}

static void PQ_Insert_null_parameter() {
  TestThingy t;
  PriorityQueue* sut = PQ_Create(test_comparator);

  PQResult result = PQ_Insert(NULL, &t);
  CU_ASSERT_EQUAL(PQ_NullParameter, result);

  result = PQ_Insert(sut, NULL);
  CU_ASSERT_EQUAL(PQ_NullParameter, result);

  PQ_Destroy(sut, NULL);
}

static void PQ_Insert_first_item() {
  TestThingy* t = test_obj_init(1);
  PriorityQueue* sut = PQ_Create(test_comparator);

  PQResult result = PQ_Insert(sut, t);
  CU_ASSERT_EQUAL(PQ_Success, result);

  CU_ASSERT_PTR_EQUAL(t, sut->head->payload);
  CU_ASSERT_PTR_EQUAL(t, sut->tail->payload);
  CU_ASSERT_EQUAL(1, sut->n);

  PQ_Destroy(sut, test_obj_destroy);
}

static void PQ_Insert_two_items() {
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

static void PQ_Insert_standard() {
  const int n = 5;
  PriorityQueue* sut = PQ_Create(test_comparator);

  TestThingy* t[n];
  for (int i = 0; i < n; i++) t[i] = test_obj_init(i);

  PQ_Insert(sut, t[3]);
  PQ_Insert(sut, t[1]);
  PQ_Insert(sut, t[0]);
  PQ_Insert(sut, t[2]);
  PQ_Insert(sut, t[4]);

  CU_ASSERT_EQUAL(n, sut->n);

  int tracker = 0;
  PQ_Item* curr = sut->head;

  while (curr != NULL) {
    CU_ASSERT_PTR_EQUAL(t[tracker], curr->payload);
    if (tracker == 0) {
      CU_ASSERT_PTR_NULL(curr->prev);
    } else {
      CU_ASSERT_EQUAL(tracker - 1, GetX(curr->prev));
    }

    if (tracker == n - 1) {
      CU_ASSERT_PTR_NULL(curr->next);
    } else {
      CU_ASSERT_EQUAL(tracker + 1, GetX(curr->next));
    }

    tracker++;
    curr = curr->next;
  }

  PQ_Destroy(sut, test_obj_destroy);
}

static void PQ_Remove_null_parameter() {
  ErrorReporter_Clear();
  PriorityQueue* sut = PQ_Create(test_comparator);

  void* result = PQ_Remove(NULL);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(PQ_NullParameter, ErrorReporter_LastErrorCode());

  PQ_Destroy(sut, NULL);
}

static void PQ_Remove_empty() {
  ErrorReporter_Clear();
  PriorityQueue* sut = PQ_Create(test_comparator);

  void* result = PQ_Remove(sut);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(PQ_EmptyQueue, ErrorReporter_LastErrorCode());

  PQ_Destroy(sut, NULL);
}

static void PQ_Remove_last_item() {
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

static void PQ_Remove_priority_order() {
  const size_t n = 5;

  TestThingy* expected[n];
  for (size_t i = 0; i < n; i++) expected[i] = test_obj_init(i);

  PriorityQueue* sut = PQ_Create(test_comparator);

  PQ_Insert(sut, expected[2]);
  PQ_Insert(sut, expected[4]);
  PQ_Insert(sut, expected[0]);
  PQ_Insert(sut, expected[3]);
  PQ_Insert(sut, expected[1]);

  CU_ASSERT_EQUAL(n, sut->n);
  CU_ASSERT_PTR_EQUAL(expected[0], sut->head->payload);
  CU_ASSERT_PTR_EQUAL(expected[4], sut->tail->payload);

  for (size_t i = 0; i < n; i++) CU_ASSERT_EQUAL(expected[i], PQ_Remove(sut));

  CU_ASSERT_PTR_NULL(sut->head);
  CU_ASSERT_PTR_NULL(sut->tail);
  CU_ASSERT_EQUAL(0, sut->n);

  PQ_Destroy(sut, NULL);
  for (size_t i = 0; i < n; i++) test_obj_destroy(expected[i]);
}

static void PQ_IsEmpty_null_parameter() {
  ErrorReporter_Clear();
  bool result = PQ_IsEmpty(NULL);
  CU_ASSERT_TRUE(result);
  CU_ASSERT_EQUAL(PQ_NullParameter, ErrorReporter_LastErrorCode());
}

static void PQ_IsEmpty_empty() {
  PriorityQueue* sut = PQ_Create(NULL);
  CU_ASSERT_TRUE(PQ_IsEmpty(sut));
  PQ_Destroy(sut, NULL);
}

static void PQ_IsEmpty_not_empty() {
  TestThingy* t = test_obj_init(1);
  PriorityQueue* sut = PQ_Create(test_comparator);
  PQ_Insert(sut, t);
  CU_ASSERT_FALSE(PQ_IsEmpty(sut));
  PQ_Destroy(sut, test_obj_destroy);
}

static void PQ_Peek_null_parameter() {
  ErrorReporter_Clear();
  PriorityQueue* sut = PQ_Create(test_comparator);

  void* result = PQ_Peek(NULL);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(PQ_NullParameter, ErrorReporter_LastErrorCode());

  PQ_Destroy(sut, NULL);
}

static void PQ_Peek_empty() {
  ErrorReporter_Clear();
  PriorityQueue* sut = PQ_Create(test_comparator);

  void* result = PQ_Peek(sut);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(PQ_EmptyQueue, ErrorReporter_LastErrorCode());

  PQ_Destroy(sut, NULL);
}

static void PQ_Peek_not_empty() {
  TestThingy* t = test_obj_init(1);
  PriorityQueue* sut = PQ_Create(test_comparator);
  PQ_Insert(sut, t);

  void* result = PQ_Peek(sut);

  CU_ASSERT_PTR_EQUAL(t, result);
  CU_ASSERT_EQUAL(1, sut->n);

  PQ_Destroy(sut, test_obj_destroy);
}

static void PQ_Reprioritize_null_parameter() {
  TestThingy* t = test_obj_init(1);

  PQResult result = PQ_Reprioritize(NULL, t);
  CU_ASSERT_EQUAL(PQ_NullParameter, result);

  PriorityQueue* sut = PQ_Create(test_comparator);
  result = PQ_Reprioritize(sut, NULL);
  CU_ASSERT_EQUAL(PQ_NullParameter, result);

  PQ_Destroy(sut, NULL);
  test_obj_destroy(t);
}

static void PQ_Reprioritize_not_found() {
  TestThingy* t = test_obj_init(1);
  TestThingy* not_found = test_obj_init(1);
  PriorityQueue* sut = PQ_Create(test_comparator);
  PQ_Insert(sut, t);

  PQResult result = PQ_Reprioritize(sut, not_found);
  CU_ASSERT_EQUAL(PQ_ItemNotFound, result);

  PQ_Destroy(sut, test_obj_destroy);
  test_obj_destroy(not_found);
}

static void PQ_Reprioritize_first_item() {
  PriorityQueue* sut = CreateSut();

  CU_ASSERT_PTR_EQUAL(_items[0], sut->head->payload);
  CU_ASSERT_PTR_EQUAL(_items[_n - 1], sut->tail->payload);

  _items[0]->x = 10;
  PQResult result = PQ_Reprioritize(sut, _items[0]);

  CU_ASSERT_EQUAL(PQ_Success, result);
  CU_ASSERT_PTR_EQUAL(_items[1], sut->head->payload);
  CU_ASSERT_PTR_EQUAL(_items[0], sut->tail->payload);

  PQ_Destroy(sut, test_obj_destroy);
}

static void PQ_Reprioritize_last_item() {
  PriorityQueue* sut = CreateSut();

  CU_ASSERT_PTR_EQUAL(_items[0], sut->head->payload);
  CU_ASSERT_PTR_EQUAL(_items[_n - 1], sut->tail->payload);

  _items[_n - 1]->x = 0;
  PQResult result = PQ_Reprioritize(sut, _items[_n - 1]);

  CU_ASSERT_EQUAL(PQ_Success, result);
  CU_ASSERT_PTR_EQUAL(_items[_n - 1], sut->head->payload);
  CU_ASSERT_PTR_EQUAL(_items[_n - 2], sut->tail->payload);

  PQ_Destroy(sut, test_obj_destroy);
}

static void PQ_Reprioritize_standard() {
  const int head = 3;
  const int tail = 2;
  PriorityQueue* sut = CreateSut();

  CU_ASSERT_PTR_EQUAL(_items[0], sut->head->payload);
  CU_ASSERT_PTR_EQUAL(_items[_n - 1], sut->tail->payload);

  _items[head]->x = 0;
  PQResult result = PQ_Reprioritize(sut, _items[head]);

  CU_ASSERT_EQUAL(PQ_Success, result);
  CU_ASSERT_PTR_EQUAL(_items[head], sut->head->payload);

  _items[tail]->x = _n * 2;
  result = PQ_Reprioritize(sut, _items[tail]);

  CU_ASSERT_EQUAL(PQ_Success, result);
  CU_ASSERT_PTR_EQUAL(_items[tail], sut->tail->payload);

  PQ_Destroy(sut, test_obj_destroy);
}

static void PQ_Destroy_null() {
  // No error means pass
  PQ_Destroy(NULL, NULL);
}

int RegisterPriorityQueueTests() {
  CU_TestInfo Queue_tests[] = {CU_TEST_INFO(PQ_Create_failed_malloc),
                               CU_TEST_INFO(PQ_Create_initalizes_values),
                               CU_TEST_INFO(PQ_Create_null_parameter),
                               CU_TEST_INFO(PQ_Insert_failed_malloc),
                               CU_TEST_INFO(PQ_Insert_null_parameter),
                               CU_TEST_INFO(PQ_Insert_first_item),
                               CU_TEST_INFO(PQ_Insert_two_items),
                               CU_TEST_INFO(PQ_Insert_standard),
                               CU_TEST_INFO(PQ_Remove_null_parameter),
                               CU_TEST_INFO(PQ_Remove_empty),
                               CU_TEST_INFO(PQ_Remove_last_item),
                               CU_TEST_INFO(PQ_Remove_priority_order),
                               CU_TEST_INFO(PQ_IsEmpty_null_parameter),
                               CU_TEST_INFO(PQ_IsEmpty_empty),
                               CU_TEST_INFO(PQ_IsEmpty_not_empty),
                               CU_TEST_INFO(PQ_Destroy_null),
                               CU_TEST_INFO(PQ_Peek_null_parameter),
                               CU_TEST_INFO(PQ_Peek_empty),
                               CU_TEST_INFO(PQ_Peek_not_empty),
                               CU_TEST_INFO(PQ_Reprioritize_null_parameter),
                               CU_TEST_INFO(PQ_Reprioritize_not_found),
                               CU_TEST_INFO(PQ_Reprioritize_first_item),
                               CU_TEST_INFO(PQ_Reprioritize_last_item),
                               CU_TEST_INFO(PQ_Reprioritize_standard),
                               CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Priority Queue",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Queue_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
