#include <stdbool.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "LinkedList.h"
#include "MemAllocMock.h"

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int noop(void) { return 0; }

static void ListIsValid(
    const LinkedList* list, const size_t n, void* expected[n])
{
    CU_ASSERT_EQUAL(n, list->size);

    if (list->head != NULL)
        CU_ASSERT_PTR_NULL(list->head->prev);

    if (list->tail != NULL)
        CU_ASSERT_PTR_NULL(list->tail->next);

    LinkedListItem* item = list->head;
    for (size_t i = 0; i < n; i++) {
        if (item == NULL || item->payload == NULL)
            CU_FAIL("null item")
        else {
            CU_ASSERT_PTR_EQUAL(item->payload, expected[i]);
            item = item->next;
        }
    }
    CU_ASSERT_PTR_NULL(item);

    item = list->tail;
    for (size_t i = 1; i <= n; i++) {
        if (item == NULL || item->payload == NULL)
            CU_FAIL("null item")
        else {
            CU_ASSERT_PTR_EQUAL(item->payload, expected[n - i]);
            item = item->prev;
        }
    }
    CU_ASSERT_PTR_NULL(item);
}

/*************************** LinkedList_Create
 * ****************************************/
static void LinkedList_Create_bad_malloc()
{
    MemAllocMock_InterceptMalloc(NULL);
    LinkedList* result = LinkedList_Create(free, int_comparator);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());
    MemAllocMock_ResetMalloc();
}

static void LinkedList_Create_initalizes_values()
{
    LinkedList* list = LinkedList_Create(free, int_comparator);

    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(0, list->size);
    CU_ASSERT_PTR_NULL(list->head);
    CU_ASSERT_PTR_NULL(list->tail);
    CU_ASSERT_PTR_EQUAL(free, list->freer);
    CU_ASSERT_EQUAL(int_comparator, list->comparator);

    LinkedList_Destroy(list);
}

/*************************** LinkedList_InsertAt ******************************/
static void LinkedList_InsertAt_null_parameters()
{
    ListOpResult result = LinkedList_InsertAt(NULL, NULL, 0);
    CU_ASSERT_EQUAL(ListOp_NullParameter, result);

    int dummy = 5;
    result = LinkedList_InsertAt(NULL, &dummy, 0);
    CU_ASSERT_EQUAL(ListOp_NullParameter, result);

    LinkedList* list = LinkedList_Create(NULL, int_comparator);
    result = LinkedList_InsertAt(list, NULL, 0);
    CU_ASSERT_EQUAL(ListOp_NullParameter, result);

    LinkedList_Destroy(list);
}

static void LinkedList_InsertAt_bad_index()
{
    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    int dummy = 5;
    ListOpResult result = LinkedList_InsertAt(list, &dummy, 5);
    CU_ASSERT_EQUAL(ListOp_InvalidIndex, result);

    LinkedList_Destroy(list);
}

static void LinkedList_InsertAt_bad_malloc()
{
    int dummy = 5;
    LinkedList* list = LinkedList_Create(free, int_comparator);

    MemAllocMock_InterceptMalloc(NULL);
    ListOpResult result = LinkedList_InsertAt(list, &dummy, 0);

    CU_ASSERT_EQUAL(ListOp_FailedMalloc, result);
    CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());

    MemAllocMock_ResetMalloc();
    LinkedList_Destroy(list);
}

static void LinkedList_InsertAt_empty()
{
    int payload = 138;
    void* expected[] = { &payload };
    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    ListOpResult result = LinkedList_InsertAt(list, &payload, 0);

    CU_ASSERT_EQUAL(ListOp_Success, result);
    ListIsValid(list, 1, expected);

    LinkedList_Destroy(list);
}

static void LinkedList_InsertAt_tail()
{
    int head_payload = 138;
    int tail_payload = 1138;
    void* expected[] = { &head_payload, &tail_payload };

    LinkedList* list = LinkedList_Create(NULL, int_comparator);
    LinkedList_InsertAt(list, &head_payload, 0);
    ListOpResult result = LinkedList_InsertAt(list, &tail_payload, 1);

    CU_ASSERT_EQUAL(ListOp_Success, result);
    ListIsValid(list, 2, expected);

    LinkedList_Destroy(list);
}

static void LinkedList_InsertAt_middle()
{
    int head_payload = 138;
    int mid_payload = 1138;
    int tail_payload = 11138;
    void* expected[] = { &head_payload, &mid_payload, &tail_payload };

    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    LinkedList_InsertAt(list, &head_payload, 0);
    LinkedList_InsertAt(list, &tail_payload, 1);
    ListOpResult result = LinkedList_InsertAt(list, &mid_payload, 1);
    CU_ASSERT_EQUAL(ListOp_Success, result);

    ListIsValid(list, 3, expected);

    LinkedList_Destroy(list);
}

static void LinkedList_InsertAt_head_creates_links()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { &payload[6], &payload[5], &payload[4], &payload[3],
        &payload[2], &payload[1], &payload[0] };

    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    for (size_t i = 0; i < n; i++) {
        ListOpResult result = LinkedList_InsertAt(list, &payload[i], 0);
        CU_ASSERT_EQUAL(ListOp_Success, result);
    }

    ListIsValid(list, n, expected);

    LinkedList_Destroy(list);
}

static void LinkedList_InsertAt_tail_creates_links()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { &payload[0], &payload[1], &payload[2], &payload[3],
        &payload[4], &payload[5], &payload[6] };

    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    for (size_t i = 0; i < n; i++) {
        ListOpResult result = LinkedList_InsertAt(list, &payload[i], i);
        CU_ASSERT_EQUAL(ListOp_Success, result);
    }

    ListIsValid(list, n, expected);

    LinkedList_Destroy(list);
}

static void LinkedList_InsertAt_mid_creates_links()
{
    int payload[] = { 0, 1, 2 };
    void* expected[] = { &payload[0], &payload[2], &payload[1] };

    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    ListOpResult result = LinkedList_InsertAt(list, &payload[0], 0);
    CU_ASSERT_EQUAL(ListOp_Success, result);

    result = LinkedList_InsertAt(list, &payload[1], 1);
    CU_ASSERT_EQUAL(ListOp_Success, result);

    result = LinkedList_InsertAt(list, &payload[2], 1);
    CU_ASSERT_EQUAL(ListOp_Success, result);

    ListIsValid(list, 3, expected);

    LinkedList_Destroy(list);
}

/*************************** LinkedList_DeleteAt ******************************/
static bool is_free = false;

static void MockFreer(void* ptr)
{
    (void)ptr;
    is_free = true;
}

static void LinkedList_DeleteAt_null_parameters()
{
    ListOpResult result = LinkedList_DeleteAt(NULL, 0);
    CU_ASSERT_EQUAL(ListOp_NullParameter, result);
}

static void LinkedList_DeleteAt_invalid_index()
{
    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    ListOpResult result = LinkedList_DeleteAt(list, 1);

    CU_ASSERT_EQUAL(ListOp_InvalidIndex, result);

    LinkedList_Destroy(list);
}

static void LinkedList_DeleteAt_calls_freer()
{
    int dummy = 5;
    is_free = false;
    LinkedList* list = LinkedList_Create(MockFreer, int_comparator);

    LinkedList_InsertAt(list, &dummy, 0);
    ListOpResult result = LinkedList_DeleteAt(list, 0);

    CU_ASSERT_EQUAL(ListOp_Success, result);
    CU_ASSERT_EQUAL(true, is_free);

    LinkedList_Destroy(list);
}

static void LinkedList_DeleteAt_all_items()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { NULL };
    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    for (size_t i = 0; i < n; i++)
        LinkedList_InsertAt(list, &payload[i], 0);

    for (size_t i = 0; i < n; i++)
        LinkedList_DeleteAt(list, 0);

    ListIsValid(list, 0, expected);

    LinkedList_Destroy(list);
}

static void LinkedList_DeleteAt_head_modifies_links()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { &payload[5], &payload[4], &payload[3], &payload[2],
        &payload[1], &payload[0] };
    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    for (size_t i = 0; i < n; i++)
        LinkedList_InsertAt(list, &payload[i], 0);

    LinkedList_DeleteAt(list, 0);
    ListIsValid(list, n - 1, expected);

    LinkedList_Destroy(list);
}

static void LinkedList_DeleteAt_tail_modifies_links()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { &payload[6], &payload[5], &payload[4], &payload[3],
        &payload[2], &payload[1] };
    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    for (size_t i = 0; i < n; i++)
        LinkedList_InsertAt(list, &payload[i], 0);

    LinkedList_DeleteAt(list, n - 1);
    ListIsValid(list, n - 1, expected);

    LinkedList_Destroy(list);
}

static void LinkedList_DeleteAt_mid_modifies_links()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { &payload[6], &payload[5], &payload[3], &payload[2],
        &payload[1], &payload[0] };
    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    for (size_t i = 0; i < n; i++)
        LinkedList_InsertAt(list, &payload[i], 0);

    LinkedList_DeleteAt(list, 2);
    ListIsValid(list, n - 1, expected);

    LinkedList_Destroy(list);
}

/*************************** LinkedList_Search ********************************/
static void LinkedList_Search_null_parameters()
{
    void* result = LinkedList_Search(NULL, NULL);
    CU_ASSERT_PTR_NULL(result);

    int dummy = 5;
    result = LinkedList_Search(NULL, &dummy);
    CU_ASSERT_PTR_NULL(result);

    LinkedList* list = LinkedList_Create(free, int_comparator);
    result = LinkedList_Search(list, NULL);
    CU_ASSERT_PTR_NULL(result);

    LinkedList_Destroy(list);
}

static void LinkedList_Search_not_found()
{
    size_t n = 7;
    int dummy = 8;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    for (size_t i = 0; i < n; i++)
        LinkedList_InsertAt(list, &payload[i], 0);

    void* result = LinkedList_Search(list, &dummy);
    CU_ASSERT_PTR_NULL(result);

    LinkedList_Destroy(list);
}

static void LinkedList_Search_finds_item()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    for (size_t i = 0; i < n; i++)
        LinkedList_InsertAt(list, &payload[i], 0);

    void* result = LinkedList_Search(list, &payload[1]);
    CU_ASSERT_EQUAL(payload[1], *(int*)result);

    LinkedList_Destroy(list);
}

/*************************** LinkedList_Destory *******************************/
// If there are no errors, this passes
static void LinkedList_Destroy_null_parameter() { LinkedList_Destroy(NULL); }

static void LinkedList_Destroy_calls_freeme()
{
    LinkedList* list = LinkedList_Create(MockFreer, int_comparator);

    is_free = false;
    int dummy = 5;
    LinkedList_InsertAt(list, &dummy, 0);

    LinkedList_Destroy(list);
    CU_ASSERT_EQUAL(true, is_free);
}

static void LinkedList_Destroy_null_freer()
{
    // If this does not create an error, it works
    LinkedList* list = LinkedList_Create(NULL, int_comparator);

    is_free = false;
    int dummy = 5;
    LinkedList_InsertAt(list, &dummy, 0);

    LinkedList_Destroy(list);
}

int register_linked_list_tests()
{
    CU_TestInfo create_tests[] = { CU_TEST_INFO(LinkedList_Create_bad_malloc),
        CU_TEST_INFO(LinkedList_Create_initalizes_values), CU_TEST_INFO_NULL };

    CU_TestInfo insert_tests[]
        = { CU_TEST_INFO(LinkedList_InsertAt_null_parameters),
              CU_TEST_INFO(LinkedList_InsertAt_bad_index),
              CU_TEST_INFO(LinkedList_InsertAt_empty),
              CU_TEST_INFO(LinkedList_InsertAt_bad_malloc),
              CU_TEST_INFO(LinkedList_InsertAt_tail),
              CU_TEST_INFO(LinkedList_InsertAt_middle),
              CU_TEST_INFO(LinkedList_InsertAt_head_creates_links),
              CU_TEST_INFO(LinkedList_InsertAt_tail_creates_links),
              CU_TEST_INFO(LinkedList_InsertAt_mid_creates_links),
              CU_TEST_INFO_NULL };

    CU_TestInfo delete_tests[]
        = { CU_TEST_INFO(LinkedList_DeleteAt_null_parameters),
              CU_TEST_INFO(LinkedList_DeleteAt_invalid_index),
              CU_TEST_INFO(LinkedList_DeleteAt_calls_freer),
              CU_TEST_INFO(LinkedList_DeleteAt_all_items),
              CU_TEST_INFO(LinkedList_DeleteAt_head_modifies_links),
              CU_TEST_INFO(LinkedList_DeleteAt_tail_modifies_links),
              CU_TEST_INFO(LinkedList_DeleteAt_mid_modifies_links),
              CU_TEST_INFO_NULL };

    CU_TestInfo search_tests[]
        = { CU_TEST_INFO(LinkedList_Search_null_parameters),
              CU_TEST_INFO(LinkedList_Search_not_found),
              CU_TEST_INFO(LinkedList_Search_finds_item), CU_TEST_INFO_NULL };

    CU_TestInfo destroy_tests[] = { CU_TEST_INFO(
                                        LinkedList_Destroy_null_parameter),
        CU_TEST_INFO(LinkedList_Destroy_null_freer),
        CU_TEST_INFO(LinkedList_Destroy_calls_freeme), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "LinkedList_Create",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = create_tests },
        { .pName = "LinkedList_InsertAt",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = insert_tests },
        { .pName = "LinkedList_DeleteAt",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = delete_tests },
        { .pName = "LinkedList_Search",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = search_tests },
        { .pName = "LinkedList_Destroy",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = destroy_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
