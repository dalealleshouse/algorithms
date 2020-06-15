#include "./heap.h"

#include <stdint.h>
#include <stdlib.h>

#include "../utils/error_reporter.h"
#include "../utils/malloc_test_wrapper.h"
#include "../utils/test_helpers.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#define SUT(size, code_block)               \
  {                                         \
    Heap* sut = CreateSut(size);            \
                                            \
    code_block;                             \
                                            \
    Heap_Destroy(sut, TestHeapObj_Destroy); \
  }

typedef struct TestHeapObj {
  int id;
  int priority;
} TestHeapObj;

static TestHeapObj* TestHeapObj_Create(int priority) {
  TestHeapObj* obj = malloc(sizeof(TestHeapObj));
  obj->id = priority;
  obj->priority = priority * 100;

  return obj;
}

static void TestHeapObj_Destroy(void* obj) { free(obj); }

static void HeapIsValid(Heap* sut) {
  CU_ASSERT_PTR_NOT_NULL(sut);

  for (size_t i = 1; i < sut->n; i++) {
    size_t parent_index = (i - 1) >> 1;

    TestHeapObj* item = sut->data[i];
    TestHeapObj* parent = sut->data[parent_index];

    if (parent->priority > item->priority) {
      printf("\nparent=%zu-%d, child=%zu-%d\n", parent_index, parent->priority,
             i, item->priority);
      CU_FAIL("Invalid Heap Value");
    }
  }
}

static int TestComparator(const void* x, const void* y) {
  TestHeapObj* _x = (TestHeapObj*)x;
  TestHeapObj* _y = (TestHeapObj*)y;

  return _y->priority - _x->priority;
}

static Heap* CreateSut(size_t size) {
  Heap* sut = Heap_Create(size, TestComparator);
  for (size_t i = 1; i <= size; i++) Heap_Insert(sut, TestHeapObj_Create(i));

  return sut;
}

static void Heap_Create_null_parameter() {
  ErrorReporter_Clear();
  Heap* sut = Heap_Create(5, NULL);

  CU_ASSERT_PTR_NULL(sut);
  CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), HeapkNullParameter);
}

static void Heap_Create_invalid_size() {
  ErrorReporter_Clear();
  Heap* sut = Heap_Create(0, TestComparator);

  CU_ASSERT_PTR_NULL(sut);
  CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), HeapInvalidSize);
}

static void Heap_Create_malloc_tester(void) {
  Heap* sut = Heap_Create(5, TestComparator);

  CU_ASSERT_PTR_NULL(sut);
  CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), HeapkFailedMemoryAllocation);
}

static void Heap_Create_failed_malloc() {
  FAILED_MALLOC_TEST({ Heap_Create_malloc_tester(); });
}

static void Heap_Create_intializes_variables() {
  const size_t size = 10;
  Heap* sut = Heap_Create(size, TestComparator);

  CU_ASSERT_PTR_NOT_NULL(sut);
  CU_ASSERT_EQUAL(0, sut->n);
  CU_ASSERT_EQUAL(size, sut->size);
  CU_ASSERT_PTR_EQUAL(TestComparator, sut->comparator);
  CU_ASSERT_PTR_NOT_NULL(sut->item_tracker);

  Heap_Destroy(sut, NULL);
}

static void Heap_Create_arith_overflow() {
  ErrorReporter_Clear();
  Heap* sut = Heap_Create(SIZE_MAX, TestComparator);
  CU_ASSERT_PTR_NULL(sut);
  CU_ASSERT_EQUAL(HeapkArithmeticOverflow, ErrorReporter_LastErrorCode());
}

static void Heap_Insert_null_parameter() {
  const size_t size = 10;
  TestHeapObj obj = {1, 1};
  Heap* sut = Heap_Create(size, TestComparator);

  HeapResult result = Heap_Insert(sut, NULL);
  CU_ASSERT_EQUAL(HeapkNullParameter, result);

  result = Heap_Insert(NULL, &obj);
  CU_ASSERT_EQUAL(HeapkNullParameter, result);

  result = Heap_Insert(NULL, NULL);
  CU_ASSERT_EQUAL(HeapkNullParameter, result);

  Heap_Destroy(sut, NULL);
}

static void Heap_Insert_first_item() {
  const size_t size = 10;
  TestHeapObj obj = {1, 1};
  Heap* sut = Heap_Create(size, TestComparator);

  HeapResult result = Heap_Insert(sut, &obj);

  CU_ASSERT_EQUAL(HeapkSuccess, result);
  CU_ASSERT_PTR_EQUAL(&obj, sut->data[0]);
  CU_ASSERT_EQUAL(1, sut->n);

  Heap_Destroy(sut, NULL);
}

static void Heap_Insert_many_items() {
  const size_t size = 11;

  Heap* sut = Heap_Create(size, TestComparator);

  for (size_t i = size; i > 0; i--) Heap_Insert(sut, TestHeapObj_Create(i));

  CU_ASSERT_EQUAL(size, sut->n);
  HeapIsValid(sut);
  Heap_Destroy(sut, TestHeapObj_Destroy);
}

static void Heap_Insert_over_size() {
  Heap* sut = CreateSut(3);

  TestHeapObj* one_more = TestHeapObj_Create(1000);
  HeapResult result = Heap_Insert(sut, one_more);

  CU_ASSERT_EQUAL(HeapOverflow, result);

  TestHeapObj_Destroy(one_more);
  Heap_Destroy(sut, TestHeapObj_Destroy);
}

static void Heap_Extract_null_parameter() {
  ErrorReporter_Clear();

  void* result = Heap_Extract(NULL);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), HeapkNullParameter);
}

static void Heap_Extract_returns_highest_priority() {
  const size_t size = 11;
  Heap* sut = CreateSut(size);

  for (int i = 1; i <= (int)size; i++) {
    TestHeapObj* obj = Heap_Extract(sut);
    CU_ASSERT_EQUAL(i * 100, obj->priority);
    CU_ASSERT_EQUAL(size - i, sut->n);
    HeapIsValid(sut);
    TestHeapObj_Destroy(obj);
  }

  Heap_Destroy(sut, TestHeapObj_Destroy);
}

static void Heap_Extract_empty_heap() {
  ErrorReporter_Clear();
  Heap* sut = Heap_Create(10, TestComparator);

  void* result = Heap_Extract(sut);

  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(HeapkEmpty, ErrorReporter_LastErrorCode());

  Heap_Destroy(sut, NULL);
}

static void Heap_IskEmpty_return_true_when_empty() {
  Heap* sut = Heap_Create(10, TestComparator);

  bool result = Heap_IskEmpty(sut);
  CU_ASSERT_TRUE(result);

  Heap_Destroy(sut, NULL);
}

static void Heap_IskEmpty_return_false_when_not_empty() {
  Heap* sut = CreateSut(2);

  bool result = Heap_IskEmpty(sut);
  CU_ASSERT_FALSE(result);

  Heap_Destroy(sut, TestHeapObj_Destroy);
}

static void Heap_IskEmpty_null_parameter() {
  ErrorReporter_Clear();

  bool result = Heap_IskEmpty(NULL);
  CU_ASSERT_TRUE(result);
  CU_ASSERT_EQUAL(HeapkNullParameter, ErrorReporter_LastErrorCode());
}

static void Heap_Peek_null_parameter() {
  ErrorReporter_Clear();

  void* result = Heap_Peek(NULL);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(HeapkNullParameter, ErrorReporter_LastErrorCode());
}

static void Heap_Peek_returns_highest_priority() {
  const size_t size = 11;
  Heap* sut = CreateSut(size);

  TestHeapObj* obj = Heap_Peek(sut);
  CU_ASSERT_EQUAL(100, obj->priority);
  CU_ASSERT_EQUAL(size, sut->n);
  HeapIsValid(sut);

  Heap_Destroy(sut, TestHeapObj_Destroy);
}

static void Heap_Peek_empty() {
  ErrorReporter_Clear();
  Heap* sut = Heap_Create(10, TestComparator);

  void* result = Heap_Peek(sut);

  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(HeapkEmpty, ErrorReporter_LastErrorCode());

  Heap_Destroy(sut, NULL);
}

static void Heap_Resize_null_parameter() {
  HeapResult result = Heap_Resize(NULL, 5);
  CU_ASSERT_EQUAL(HeapkNullParameter, result);
}

static void Heap_Resize_zero() {
  SUT(10, {
    HeapResult result = Heap_Resize(sut, 0);
    CU_ASSERT_EQUAL(HeapInvalidSize, result);
  });
}

static void Heap_Resize_too_small() {
  SUT(10, {
    HeapResult result = Heap_Resize(sut, 9);
    CU_ASSERT_EQUAL(HeapInvalidSize, result);
  });
}

static void Heap_Resize_arith_overflow() {
  SUT(10, {
    ErrorReporter_Clear();
    HeapResult result = Heap_Resize(sut, SIZE_MAX);
    CU_ASSERT_EQUAL(HeapkArithmeticOverflow, result);
  });
}

static void Heap_Resize_failed_malloc() {
  SUT(10, {
    FAILED_MALLOC_TEST({
      HeapResult result = Heap_Resize(sut, 20);
      CU_ASSERT_EQUAL(HeapkFailedMemoryAllocation, result);
    });
  });
}

static void Heap_Resize_smaller_happy_path() {
  SUT(10, {
    void* item = Heap_Extract(sut);
    TestHeapObj_Destroy(item);
    HeapResult result = Heap_Resize(sut, 9);

    CU_ASSERT_EQUAL(HeapkSuccess, result);
    CU_ASSERT_EQUAL(9, sut->size);
  });
}

static void Heap_Resize_larger_happy_path() {
  SUT(10, {
    HeapResult result = Heap_Resize(sut, 20);
    CU_ASSERT_EQUAL(HeapkSuccess, result);
    CU_ASSERT_EQUAL(20, sut->size);

    TestHeapObj* obj = TestHeapObj_Create(5);
    result = Heap_Insert(sut, obj);
    CU_ASSERT_EQUAL(HeapkSuccess, result);
    CU_ASSERT_EQUAL(11, sut->n);
  });
}

static void Heap_Destroy_null_parameter() { Heap_Destroy(NULL, free); }

static void Heap_Exists_null_parameter() {
  bool result = Heap_Exists(NULL, NULL);
  CU_ASSERT_FALSE(result);
  CU_ASSERT_EQUAL(HeapkNullParameter, ErrorReporter_LastErrorCode());

  int somePointer;
  result = Heap_Exists(NULL, &somePointer);
  CU_ASSERT_FALSE(result);
  CU_ASSERT_EQUAL(HeapkNullParameter, ErrorReporter_LastErrorCode());

  SUT(10, {
    result = Heap_Exists(sut, NULL);
    CU_ASSERT_FALSE(result);
    CU_ASSERT_EQUAL(HeapkNullParameter, ErrorReporter_LastErrorCode());
  });
}

static void Heap_Exists_returns_true_when_item_exists() {
  ErrorReporter_Clear();
  SUT(10, {
    bool exists = Heap_Exists(sut, sut->data[5]);
    CU_ASSERT_TRUE(exists);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), 0);
  });
}

static void Heap_Exists_returns_false_when_item_does_not_exist() {
  ErrorReporter_Clear();
  SUT(10, {
    int somePointer;
    bool exists = Heap_Exists(sut, &somePointer);
    CU_ASSERT_FALSE(exists);
    CU_ASSERT_EQUAL(ErrorReporter_LastErrorCode(), 0);
  });
}

static void Heap_Reproiritize_null_parameter() {
  HeapResult result = Heap_Reproiritize(NULL, NULL);
  CU_ASSERT_EQUAL(result, HeapkNullParameter);

  int somePointer;
  result = Heap_Reproiritize(NULL, &somePointer);
  CU_ASSERT_EQUAL(result, HeapkNullParameter);

  SUT(10, {
    result = Heap_Reproiritize(sut, NULL);
    CU_ASSERT_EQUAL(result, HeapkNullParameter);
  });
}

static void Heap_Reproiritize_item_not_found() {
  SUT(10, {
    int somePointer;
    HeapResult result = Heap_Reproiritize(sut, &somePointer);
    CU_ASSERT_EQUAL(result, HeapItemkNotFound);
    HeapIsValid(sut);
  });
}

static void Heap_Reproiritize_no_change_to_priority() {
  SUT(10, {
    HeapResult result = Heap_Reproiritize(sut, sut->data[5]);
    CU_ASSERT_EQUAL(result, HeapkSuccess);
    HeapIsValid(sut);
  });
}

static void Heap_Reproiritize_raise_priority() {
  SUT(10, {
    TestHeapObj* changeMe = sut->data[5];
    changeMe->priority = 1;
    HeapResult result = Heap_Reproiritize(sut, changeMe);
    CU_ASSERT_EQUAL(result, HeapkSuccess);
    HeapIsValid(sut);
  });
}

static void Heap_Reproiritize_lower_priority() {
  SUT(10, {
    TestHeapObj* changeMe = sut->data[2];
    changeMe->priority = 1200;
    HeapResult result = Heap_Reproiritize(sut, changeMe);
    CU_ASSERT_EQUAL(result, HeapkSuccess);
    HeapIsValid(sut);
  });
}

static void Heap_Reproiritize_lower_last_item() {
  SUT(10, {
    TestHeapObj* changeMe = sut->data[9];
    changeMe->priority = 1200;
    HeapResult result = Heap_Reproiritize(sut, changeMe);
    CU_ASSERT_EQUAL(result, HeapkSuccess);
    HeapIsValid(sut);
  });
}

static void Heap_Reproiritize_raise_first_item() {
  SUT(10, {
    TestHeapObj* changeMe = sut->data[0];
    changeMe->priority = 50;
    HeapResult result = Heap_Reproiritize(sut, changeMe);
    CU_ASSERT_EQUAL(result, HeapkSuccess);
    HeapIsValid(sut);
  });
}

int RegisterHeapTests() {
  CU_TestInfo Heap_tests[] = {
      CU_TEST_INFO(Heap_Create_null_parameter),
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
      CU_TEST_INFO(Heap_IskEmpty_return_true_when_empty),
      CU_TEST_INFO(Heap_IskEmpty_return_false_when_not_empty),
      CU_TEST_INFO(Heap_IskEmpty_null_parameter),
      CU_TEST_INFO(Heap_Peek_null_parameter),
      CU_TEST_INFO(Heap_Peek_returns_highest_priority),
      CU_TEST_INFO(Heap_Peek_empty),
      CU_TEST_INFO(Heap_Extract_empty_heap),
      CU_TEST_INFO(Heap_Resize_null_parameter),
      CU_TEST_INFO(Heap_Resize_zero),
      CU_TEST_INFO(Heap_Resize_too_small),
      CU_TEST_INFO(Heap_Resize_arith_overflow),
      CU_TEST_INFO(Heap_Resize_failed_malloc),
      CU_TEST_INFO(Heap_Resize_smaller_happy_path),
      CU_TEST_INFO(Heap_Resize_larger_happy_path),
      CU_TEST_INFO(Heap_Exists_null_parameter),
      CU_TEST_INFO(Heap_Exists_returns_true_when_item_exists),
      CU_TEST_INFO(Heap_Exists_returns_false_when_item_does_not_exist),
      CU_TEST_INFO(Heap_Reproiritize_null_parameter),
      CU_TEST_INFO(Heap_Reproiritize_item_not_found),
      CU_TEST_INFO(Heap_Reproiritize_no_change_to_priority),
      CU_TEST_INFO(Heap_Reproiritize_raise_priority),
      CU_TEST_INFO(Heap_Reproiritize_lower_priority),
      CU_TEST_INFO(Heap_Reproiritize_lower_last_item),
      CU_TEST_INFO(Heap_Reproiritize_raise_first_item),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "Heap",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Heap_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
