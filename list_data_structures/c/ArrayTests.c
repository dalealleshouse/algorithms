#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "Array.h"
#include "MemAllocMock.h"

#define INT2VOIDP(i) (void*)(uintptr_t)(i)
#define VOIDP2INT(i) *(int*)(i)
#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int noop(void) { return 0; }

/*************************** Array_Create *************************************/
static void Array_Create_bad_malloc()
{
    MemAllocMock_InterceptMalloc(NULL);
    Array* result = Array_Create(int_comparator, sizeof(int));
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());
    MemAllocMock_ResetMalloc();
}

static void Array_Create_inits_values()
{
    Array* result = Array_Create(int_comparator, sizeof(int));

    CU_ASSERT_PTR_EQUAL(int_comparator, result->comparator);
    CU_ASSERT_EQUAL(sizeof(int), result->item_size);
    CU_ASSERT_EQUAL(0, result->n);
    CU_ASSERT_PTR_NULL(result->array);

    Array_Destroy(result);
}

/*************************** Array_Insert *************************************/
static void Array_Insert_null_paramter()
{
    int dummy = 5;
    ListOpResult result = Array_Insert(NULL, &dummy);

    CU_ASSERT_EQUAL(ListOp_NullParameter, result);

    Array* array = Array_Create(int_comparator, sizeof(int));
    result = Array_Insert(array, NULL);
    CU_ASSERT_EQUAL(ListOp_NullParameter, result);
    CU_ASSERT_EQUAL(0, array->n);

    Array_Destroy(array);
}

static void Array_Insert_bad_malloc()
{
    int first = 1;

    Array* array = Array_Create(int_comparator, sizeof(int));

    MemAllocMock_InterceptMalloc(NULL);
    ListOpResult result = Array_Insert(array, &first);

    CU_ASSERT_EQUAL(ListOp_FailedMalloc, result);
    CU_ASSERT_EQUAL(0, array->n);
    CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());
    MemAllocMock_ResetMalloc();

    Array_Destroy(array);
}

static void Array_Insert_first_item()
{
    int first = 1;

    Array* array = Array_Create(int_comparator, sizeof(int));
    ListOpResult result = Array_Insert(array, &first);

    CU_ASSERT_EQUAL(ListOp_Success, result);
    CU_ASSERT_EQUAL(1, array->n);
    int* a = (int*)array->array;
    CU_ASSERT_EQUAL(first, a[0]);

    Array_Destroy(array);
}

static void Array_Insert_standard()
{
    const size_t n = 5;
    const int items[] = { 1, 2, 3, 4, 5 };
    const int expected[] = { 5, 4, 3, 2, 1 };

    Array* array = Array_Create(int_comparator, sizeof(int));

    for (size_t i = 0; i < n; i++) {
        ListOpResult result = Array_Insert(array, &items[i]);
        CU_ASSERT_EQUAL(ListOp_Success, result);
    }

    CU_ASSERT_EQUAL(n, array->n);
    CU_ASSERT_EQUAL(0, memcmp(expected, array->array, sizeof(int) * n));

    Array_Destroy(array);
}

static void Array_Insert_bad_malloc_on_realloc()
{
    int first = 1;
    int second = 2;

    Array* array = Array_Create(int_comparator, sizeof(int));
    Array_Insert(array, &first);

    MemAllocMock_InterceptMalloc(NULL);
    ListOpResult result = Array_Insert(array, &second);

    CU_ASSERT_EQUAL(ListOp_FailedMalloc, result);
    CU_ASSERT_EQUAL(1, array->n);
    CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());
    MemAllocMock_ResetMalloc();

    Array_Destroy(array);
}

/*************************** Array_Search *************************************/
static void Array_Search_null_parameter()
{
    void* result = Array_Search(NULL, NULL);
    CU_ASSERT_PTR_NULL(result);

    Array* array = Array_Create(int_comparator, sizeof(int));
    result = Array_Search(array, NULL);
    CU_ASSERT_PTR_NULL(result);

    int dummy = 5;
    result = Array_Search(NULL, &dummy);
    CU_ASSERT_PTR_NULL(result);

    Array_Destroy(array);
}

static void Array_Search_not_found()
{
    const size_t n = 5;
    const int dummy = 10;
    const int items[] = { 1, 2, 3, 4, 5 };

    Array* array = Array_Create(int_comparator, sizeof(int));

    for (size_t i = 0; i < n; i++) {
        ListOpResult result = Array_Insert(array, &items[i]);
        CU_ASSERT_EQUAL(ListOp_Success, result);
    }

    void* result = Array_Search(array, &dummy);
    CU_ASSERT_PTR_NULL(result);

    Array_Destroy(array);
}

static void Array_Search_standard()
{
    const size_t n = 5;
    const int find = 2;
    const int find2 = 4;
    const int items[] = { 1, 2, 3, 4, 5 };

    Array* array = Array_Create(int_comparator, sizeof(int));

    for (size_t i = 0; i < n; i++) {
        ListOpResult result = Array_Insert(array, &items[i]);
        CU_ASSERT_EQUAL(ListOp_Success, result);
    }

    void* result = Array_Search(array, &find);
    CU_ASSERT_PTR_EQUAL(result, (char*)array->array + (array->item_size * 3));

    result = Array_Search(array, &find2);
    CU_ASSERT_PTR_EQUAL(result, (char*)array->array + array->item_size);

    Array_Destroy(array);
}

/*************************** Array_Enumerate **********************************/
static const size_t mock_n = 13;
static int mock_vals[] = { 50, 25, 33, 11, 30, 40, 75, 61, 89, 52, 82, 95, 55 };
static int mock_pos = 0;
static void MockItemHandler(void* item)
{
    // The items are inserted into the head, so they are backwards from how they
    // are inserted
    CU_ASSERT_EQUAL(mock_vals[mock_n - 1 - mock_pos], VOIDP2INT(item));
    mock_pos++;
}

static void Array_Enumerate_null_parameter()
{
    Array* array = Array_Create(int_comparator, sizeof(int));

    ListOpResult result = Array_Enumerate(array, NULL);
    CU_ASSERT_EQUAL(ListOp_NullParameter, result);

    result = Array_Enumerate(NULL, MockItemHandler);
    CU_ASSERT_EQUAL(ListOp_NullParameter, result);

    Array_Destroy(array);
}

static void Array_Enumerate_empty()
{
    Array* array = Array_Create(int_comparator, sizeof(int));

    ListOpResult result = Array_Enumerate(array, MockItemHandler);
    CU_ASSERT_EQUAL(ListOp_Success, result);

    Array_Destroy(array);
}

static void Array_Enumerate_standard()
{
    Array* array = Array_Create(int_comparator, sizeof(int));

    for (size_t i = 0; i < mock_n; i++)
        Array_Insert(array, &mock_vals[i]);

    mock_pos = 0;
    ListOpResult result = Array_Enumerate(array, MockItemHandler);
    CU_ASSERT_EQUAL(ListOp_Success, result);
    CU_ASSERT_EQUAL(mock_n, mock_pos);

    Array_Destroy(array);
}

/*************************** Array_Destory ************************************/
static void Array_Destroy_null_parameter()
{
    // if no error, this is cool
    Array_Destroy(NULL);
}

int register_array_tests()
{
    CU_TestInfo Create_tests[] = { CU_TEST_INFO(Array_Create_bad_malloc),
        CU_TEST_INFO(Array_Create_inits_values), CU_TEST_INFO_NULL };

    CU_TestInfo Insert_tests[] = { CU_TEST_INFO(Array_Insert_null_paramter),
        CU_TEST_INFO(Array_Insert_bad_malloc),
        CU_TEST_INFO(Array_Insert_first_item),
        CU_TEST_INFO(Array_Insert_standard),
        CU_TEST_INFO(Array_Insert_bad_malloc_on_realloc), CU_TEST_INFO_NULL };

    CU_TestInfo Search_tests[] = { CU_TEST_INFO(Array_Search_null_parameter),
        CU_TEST_INFO(Array_Search_not_found),
        CU_TEST_INFO(Array_Search_standard), CU_TEST_INFO_NULL };

    CU_TestInfo Enumerate_tests[]
        = { CU_TEST_INFO(Array_Enumerate_null_parameter),
              CU_TEST_INFO(Array_Enumerate_empty),
              CU_TEST_INFO(Array_Enumerate_standard), CU_TEST_INFO_NULL };

    CU_TestInfo Destroy_tests[]
        = { CU_TEST_INFO(Array_Destroy_null_parameter), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "Array_Create",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Create_tests },
        { .pName = "Array_Insert",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Insert_tests },
        { .pName = "Search_Insert",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Search_tests },
        { .pName = "Enumerate_Insert",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Enumerate_tests },
        { .pName = "Destroy_Insert",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Destroy_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}