// Copyright 2020 Hideous Humpback Freak https://hideoushumpbackfreak.com/
#include "./linked_list.h"

#include <stdbool.h>
#include <stdlib.h>

#include "../utils/malloc_test_wrapper.h"
#include "../utils/math.h"
#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#define SUT(code_block)                                                     \
  {                                                                         \
    LinkedList* sut = NULL;                                                 \
    ResultCode result_code = LinkedList_Create(NULL, PIntComparator, &sut); \
    CU_ASSERT_EQUAL(result_code, kSuccess);                                 \
    code_block;                                                             \
    LinkedList_Destroy(sut);                                                \
  }

static void ListIsValid(const LinkedList* list, const size_t n,
                        void* expected[n]) {
  CU_ASSERT_EQUAL(n, list->size);

  if (list->head != NULL) CU_ASSERT_PTR_NULL(list->head->prev);

  if (list->tail != NULL) CU_ASSERT_PTR_NULL(list->tail->next);

  LinkedListItem* item = list->head;
  for (size_t i = 0; i < n; i++) {
    if (item == NULL || item->payload == NULL) {
      CU_FAIL("null item")
    } else {
      CU_ASSERT_PTR_EQUAL(item->payload, expected[i]);
      item = item->next;
    }
  }
  CU_ASSERT_PTR_NULL(item);

  item = list->tail;
  for (size_t i = 1; i <= n; i++) {
    if (item == NULL || item->payload == NULL) {
      CU_FAIL("null item")
    } else {
      CU_ASSERT_PTR_EQUAL(item->payload, expected[n - i]);
      item = item->prev;
    }
  }
  CU_ASSERT_PTR_NULL(item);
}

/*************************** LinkedList_Create ********************************/
static void LinkedList_Create_bad_malloc() {
#if !defined(NDEBUG)
  FAILED_MALLOC_TEST({
    LinkedList* list = NULL;
    ResultCode result_code = LinkedList_Create(free, PIntComparator, &list);

    CU_ASSERT_PTR_NULL(list);
    CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
  })
#endif
}

static void LinkedList_Create_initalizes_values() {
  LinkedList* list = NULL;
  ResultCode result_code = LinkedList_Create(free, PIntComparator, &list);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  if (list == NULL) {
    CU_FAIL("list is null");
    return;
  }

  CU_ASSERT_EQUAL(0, list->size);
  CU_ASSERT_PTR_NULL(list->head);
  CU_ASSERT_PTR_NULL(list->tail);
  CU_ASSERT_PTR_EQUAL(free, list->freer);
  CU_ASSERT_EQUAL(PIntComparator, list->comparator);

  LinkedList_Destroy(list);
}

/*************************** LinkedList_InsertAt ******************************/
static void LinkedList_InsertAt_null_parameters() {
  ResultCode result = LinkedList_InsertAt(NULL, NULL, 0);
  CU_ASSERT_EQUAL(kNullParameter, result);

  int dummy = 5;
  result = LinkedList_InsertAt(NULL, &dummy, 0);
  CU_ASSERT_EQUAL(kNullParameter, result);

  LinkedList* list = NULL;
  ResultCode result_code = LinkedList_Create(NULL, PIntComparator, &list);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  result = LinkedList_InsertAt(list, NULL, 0);
  CU_ASSERT_EQUAL(kNullParameter, result);

  LinkedList_Destroy(list);
}

static void LinkedList_InsertAt_bad_index() {
  SUT({
    int dummy = 5;
    ResultCode result = LinkedList_InsertAt(sut, &dummy, 5);
    CU_ASSERT_EQUAL(kInvalidIndex, result);
  });
}

static void LinkedList_InsertAt_bad_malloc() {
  int dummy = 5;
  LinkedList* sut = NULL;
  ResultCode result_code = LinkedList_Create(free, PIntComparator, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  FAILED_MALLOC_TEST({
    ResultCode result = LinkedList_InsertAt(sut, &dummy, 0);
    CU_ASSERT_EQUAL(kFailedMemoryAllocation, result);
  })

  LinkedList_Destroy(sut);
}

static void LinkedList_InsertAt_empty() {
  int payload = 138;
  void* expected[] = {&payload};

  SUT({
    ResultCode result = LinkedList_InsertAt(sut, &payload, 0);

    CU_ASSERT_EQUAL(kSuccess, result);
    ListIsValid(sut, 1, expected);
  })
}

static void LinkedList_InsertAt_tail() {
  int head_payload = 138;
  int tail_payload = 1138;
  void* expected[] = {&head_payload, &tail_payload};

  SUT({
    LinkedList_InsertAt(sut, &head_payload, 0);
    ResultCode result = LinkedList_InsertAt(sut, &tail_payload, 1);

    CU_ASSERT_EQUAL(kSuccess, result);
    ListIsValid(sut, 2, expected);
  })
}

static void LinkedList_InsertAt_middle() {
  int head_payload = 138;
  int mid_payload = 1138;
  int tail_payload = 11138;
  void* expected[] = {&head_payload, &mid_payload, &tail_payload};

  SUT({
    LinkedList_InsertAt(sut, &head_payload, 0);
    LinkedList_InsertAt(sut, &tail_payload, 1);
    ResultCode result = LinkedList_InsertAt(sut, &mid_payload, 1);
    CU_ASSERT_EQUAL(result, kSuccess);
    ListIsValid(sut, 3, expected);
  });
}

static void LinkedList_InsertAt_head_creates_links() {
  size_t n = 7;
  int payload[] = {0, 1, 2, 3, 4, 5, 6};
  void* expected[] = {&payload[6], &payload[5], &payload[4], &payload[3],
                      &payload[2], &payload[1], &payload[0]};

  SUT({
    for (size_t i = 0; i < n; i++) {
      ResultCode result = LinkedList_InsertAt(sut, &payload[i], 0);
      CU_ASSERT_EQUAL(kSuccess, result);
    }

    ListIsValid(sut, n, expected);
  });
}

static void LinkedList_InsertAt_tail_creates_links() {
  size_t n = 7;
  int payload[] = {0, 1, 2, 3, 4, 5, 6};
  void* expected[] = {&payload[0], &payload[1], &payload[2], &payload[3],
                      &payload[4], &payload[5], &payload[6]};

  SUT({
    for (size_t i = 0; i < n; i++) {
      ResultCode result = LinkedList_InsertAt(sut, &payload[i], i);
      CU_ASSERT_EQUAL(kSuccess, result);
    }

    ListIsValid(sut, n, expected);
  });
}

static void LinkedList_InsertAt_mid_creates_links() {
  int payload[] = {0, 1, 2};
  void* expected[] = {&payload[0], &payload[2], &payload[1]};

  SUT({
    ResultCode result = LinkedList_InsertAt(sut, &payload[0], 0);
    CU_ASSERT_EQUAL(kSuccess, result);

    result = LinkedList_InsertAt(sut, &payload[1], 1);
    CU_ASSERT_EQUAL(kSuccess, result);

    result = LinkedList_InsertAt(sut, &payload[2], 1);
    CU_ASSERT_EQUAL(kSuccess, result);

    ListIsValid(sut, 3, expected);
  });
}

/*************************** LinkedList_DeleteAt ******************************/
static bool is_free = false;

static void MockFreer(void* ptr) {
  (void)ptr;
  is_free = true;
}

static void LinkedList_DeleteAt_null_parameters() {
  ResultCode result = LinkedList_DeleteAt(NULL, 0);
  CU_ASSERT_EQUAL(kNullParameter, result);
}

static void LinkedList_DeleteAt_invalid_index() {
  SUT({
    ResultCode result = LinkedList_DeleteAt(sut, 1);
    CU_ASSERT_EQUAL(kInvalidIndex, result);
  });
}

static void LinkedList_DeleteAt_calls_freer() {
  int dummy = 5;
  is_free = false;
  LinkedList* sut = NULL;
  ResultCode result_code = LinkedList_Create(MockFreer, PIntComparator, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  LinkedList_InsertAt(sut, &dummy, 0);
  ResultCode result = LinkedList_DeleteAt(sut, 0);

  CU_ASSERT_EQUAL(kSuccess, result);
  CU_ASSERT_EQUAL(true, is_free);

  LinkedList_Destroy(sut);
}

static void LinkedList_DeleteAt_all_items() {
  size_t n = 7;
  int payload[] = {0, 1, 2, 3, 4, 5, 6};
  void* expected[] = {NULL};
  SUT({
    for (size_t i = 0; i < n; i++) LinkedList_InsertAt(sut, &payload[i], 0);

    for (size_t i = 0; i < n; i++) LinkedList_DeleteAt(sut, 0);

    ListIsValid(sut, 0, expected);
  });
}

static void LinkedList_DeleteAt_head_modifies_links() {
  size_t n = 7;
  int payload[] = {0, 1, 2, 3, 4, 5, 6};
  void* expected[] = {&payload[5], &payload[4], &payload[3],
                      &payload[2], &payload[1], &payload[0]};
  SUT({
    for (size_t i = 0; i < n; i++) LinkedList_InsertAt(sut, &payload[i], 0);

    LinkedList_DeleteAt(sut, 0);
    ListIsValid(sut, n - 1, expected);
  });
}

static void LinkedList_DeleteAt_tail_modifies_links() {
  size_t n = 7;
  int payload[] = {0, 1, 2, 3, 4, 5, 6};
  void* expected[] = {&payload[6], &payload[5], &payload[4],
                      &payload[3], &payload[2], &payload[1]};

  SUT({
    for (size_t i = 0; i < n; i++) LinkedList_InsertAt(sut, &payload[i], 0);

    LinkedList_DeleteAt(sut, n - 1);
    ListIsValid(sut, n - 1, expected);
  });
}

static void LinkedList_DeleteAt_mid_modifies_links() {
  size_t n = 7;
  int payload[] = {0, 1, 2, 3, 4, 5, 6};
  void* expected[] = {&payload[6], &payload[5], &payload[3],
                      &payload[2], &payload[1], &payload[0]};
  SUT({
    for (size_t i = 0; i < n; i++) LinkedList_InsertAt(sut, &payload[i], 0);

    LinkedList_DeleteAt(sut, 2);
    ListIsValid(sut, n - 1, expected);
  });
}

/*************************** LinkedList_Delete ********************************/
static void LinkedList_Delete_null_parameters() {
  int val = 5;

  ResultCode result = LinkedList_Delete(NULL, NULL);
  CU_ASSERT_EQUAL(kNullParameter, result);

  result = LinkedList_Delete(NULL, &val);
  CU_ASSERT_EQUAL(kNullParameter, result);

  SUT({
    result = LinkedList_Delete(sut, NULL);
    CU_ASSERT_EQUAL(kNullParameter, result);
  });
}

static void LinkedList_Delete_not_found() {
  int val = 5;
  int not_found = 401;
  SUT({
    LinkedList_InsertAt(sut, &val, 0);
    ResultCode result = LinkedList_Delete(sut, &not_found);
    CU_ASSERT_EQUAL(result, kNotFound);
  });
}

static void LinkedList_Delete_head() {
  size_t n = 7;
  int payload[] = {0, 1, 2, 3, 4, 5, 6};
  void* expected[] = {&payload[5], &payload[4], &payload[3],
                      &payload[2], &payload[1], &payload[0]};

  SUT({
    for (size_t i = 0; i < n; i++) LinkedList_InsertAt(sut, &payload[i], 0);

    LinkedList_Delete(sut, &payload[n - 1]);
    ListIsValid(sut, n - 1, expected);
  });
}

static void LinkedList_Delete_tail() {
  size_t n = 7;
  int payload[] = {0, 1, 2, 3, 4, 5, 6};
  void* expected[] = {&payload[6], &payload[5], &payload[4],
                      &payload[3], &payload[2], &payload[1]};

  SUT({
    for (size_t i = 0; i < n; i++) LinkedList_InsertAt(sut, &payload[i], 0);

    LinkedList_Delete(sut, &payload[0]);
    ListIsValid(sut, n - 1, expected);
  });
}

static void LinkedList_Delete_mid() {
  size_t n = 7;
  int payload[] = {0, 1, 2, 3, 4, 5, 6};
  void* expected[] = {&payload[6], &payload[5], &payload[3],
                      &payload[2], &payload[1], &payload[0]};

  SUT({
    for (size_t i = 0; i < n; i++) LinkedList_InsertAt(sut, &payload[i], 0);

    LinkedList_Delete(sut, &payload[4]);
    ListIsValid(sut, n - 1, expected);
  });
}

/*************************** LinkedList_Search ********************************/
static void LinkedList_Search_null_parameters() {
  void* result = NULL;
  ResultCode result_code = LinkedList_Search(NULL, NULL, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kNullParameter);

  int dummy = 5;
  result_code = LinkedList_Search(NULL, &dummy, &result);
  CU_ASSERT_EQUAL(result_code, kNullParameter);
  CU_ASSERT_PTR_NULL(result);

  SUT({
    result_code = LinkedList_Search(sut, NULL, &result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);
    CU_ASSERT_PTR_NULL(result);
  });
}

static void LinkedList_Search_not_found() {
  size_t n = 7;
  int dummy = 8;
  int payload[] = {0, 1, 2, 3, 4, 5, 6};
  SUT({
    for (size_t i = 0; i < n; i++) LinkedList_InsertAt(sut, &payload[i], 0);

    void* result = NULL;
    ResultCode result_code = LinkedList_Search(sut, &dummy, &result);
    CU_ASSERT_EQUAL(result_code, kNotFound);
    CU_ASSERT_PTR_NULL(result);
  });
}

static void LinkedList_Search_finds_item() {
  size_t n = 7;
  int payload[] = {0, 1, 2, 3, 4, 5, 6};
  SUT({
    for (size_t i = 0; i < n; i++) LinkedList_InsertAt(sut, &payload[i], 0);

    void* result = NULL;
    ResultCode result_code = LinkedList_Search(sut, &payload[1], &result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(payload[1], *(int*)result);
  });
}

/*************************** LinkedList_Enumerate *****************************/
static const size_t mock_n = 13;
static int mock_vals[] = {50, 25, 33, 11, 30, 40, 75, 61, 89, 52, 82, 95, 55};
static int mock_pos = 0;
static void MockItemHandler(void* item) {
  CU_ASSERT_EQUAL(mock_vals[mock_pos], *(int*)item);
  mock_pos++;
}

static void LinkedList_Enumerate_null_paramter() {
  SUT({
    ResultCode result = LinkedList_Enumerate(sut, NULL);
    CU_ASSERT_EQUAL(kNullParameter, result);

    result = LinkedList_Enumerate(NULL, MockItemHandler);
    CU_ASSERT_EQUAL(kNullParameter, result);
  });
}

static void LinkedList_Enumerate_empty() {
  SUT({
    ResultCode result = LinkedList_Enumerate(sut, MockItemHandler);
    CU_ASSERT_EQUAL(kSuccess, result);
  });
}

static void LinkedList_Enumerate_standard() {
  SUT({
    for (size_t i = 0; i < mock_n; i++) {
      LinkedList_InsertAt(sut, &mock_vals[i], sut->size);
    }

    mock_pos = 0;
    ResultCode result = LinkedList_Enumerate(sut, MockItemHandler);
    CU_ASSERT_EQUAL(kSuccess, result);
    CU_ASSERT_EQUAL(mock_n, mock_pos);
  });
}

/*************************** LinkedList_Destory *******************************/
// If there are no errors, this passes
static void LinkedList_Destroy_null_parameter() { LinkedList_Destroy(NULL); }

static void LinkedList_Destroy_calls_freeme() {
  LinkedList* sut = NULL;
  ResultCode result_code = LinkedList_Create(MockFreer, PIntComparator, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  is_free = false;
  int dummy = 5;
  LinkedList_InsertAt(sut, &dummy, 0);

  LinkedList_Destroy(sut);
  CU_ASSERT_EQUAL(true, is_free);
}

static void LinkedList_Destroy_null_freer() {
  // If this does not create an error, it works
  SUT({
    is_free = false;
    int dummy = 5;
    LinkedList_InsertAt(sut, &dummy, 0);
  });
}

int RegisterLinkedListTests() {
  CU_TestInfo create_tests[] = {
      CU_TEST_INFO(LinkedList_Create_bad_malloc),
      CU_TEST_INFO(LinkedList_Create_initalizes_values), CU_TEST_INFO_NULL};

  CU_TestInfo insert_tests[] = {
      CU_TEST_INFO(LinkedList_InsertAt_null_parameters),
      CU_TEST_INFO(LinkedList_InsertAt_bad_index),
      CU_TEST_INFO(LinkedList_InsertAt_empty),
      CU_TEST_INFO(LinkedList_InsertAt_bad_malloc),
      CU_TEST_INFO(LinkedList_InsertAt_tail),
      CU_TEST_INFO(LinkedList_InsertAt_middle),
      CU_TEST_INFO(LinkedList_InsertAt_head_creates_links),
      CU_TEST_INFO(LinkedList_InsertAt_tail_creates_links),
      CU_TEST_INFO(LinkedList_InsertAt_mid_creates_links),
      CU_TEST_INFO_NULL};

  CU_TestInfo delete_at_tests[] = {
      CU_TEST_INFO(LinkedList_DeleteAt_null_parameters),
      CU_TEST_INFO(LinkedList_DeleteAt_invalid_index),
      CU_TEST_INFO(LinkedList_DeleteAt_calls_freer),
      CU_TEST_INFO(LinkedList_DeleteAt_all_items),
      CU_TEST_INFO(LinkedList_DeleteAt_head_modifies_links),
      CU_TEST_INFO(LinkedList_DeleteAt_tail_modifies_links),
      CU_TEST_INFO(LinkedList_DeleteAt_mid_modifies_links),
      CU_TEST_INFO_NULL};

  CU_TestInfo delete_tests[] = {CU_TEST_INFO(LinkedList_Delete_null_parameters),
                                CU_TEST_INFO(LinkedList_Delete_not_found),
                                CU_TEST_INFO(LinkedList_Delete_head),
                                CU_TEST_INFO(LinkedList_Delete_tail),
                                CU_TEST_INFO(LinkedList_Delete_mid),
                                CU_TEST_INFO_NULL};

  CU_TestInfo search_tests[] = {CU_TEST_INFO(LinkedList_Search_null_parameters),
                                CU_TEST_INFO(LinkedList_Search_not_found),
                                CU_TEST_INFO(LinkedList_Search_finds_item),
                                CU_TEST_INFO_NULL};

  CU_TestInfo enumerate_tests[] = {
      CU_TEST_INFO(LinkedList_Enumerate_null_paramter),
      CU_TEST_INFO(LinkedList_Enumerate_empty),
      CU_TEST_INFO(LinkedList_Enumerate_standard), CU_TEST_INFO_NULL};

  CU_TestInfo destroy_tests[] = {
      CU_TEST_INFO(LinkedList_Destroy_null_parameter),
      CU_TEST_INFO(LinkedList_Destroy_null_freer),
      CU_TEST_INFO(LinkedList_Destroy_calls_freeme), CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "LinkedList_Create",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = create_tests},
                           {.pName = "LinkedList_InsertAt",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = insert_tests},
                           {.pName = "LinkedList_DeleteAt",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = delete_at_tests},
                           {.pName = "LinkedList_Delete",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = delete_tests},
                           {.pName = "LinkedList_Search",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = search_tests},
                           {.pName = "LinkedList_Enumerate",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = enumerate_tests},
                           {.pName = "LinkedList_Destroy",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = destroy_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
