#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "linked_list.h"

#define CU_TEST_INFO(test_func)                                                \
{                                                                          \
#test_func, test_func                                                  \
}

static int is_free = 0;
static int noop(void) { return 0; }

static void _list_is_valid(const list* list, const size_t n, void* expected[n])
{
    CU_ASSERT_EQUAL(n, list->size);

    if (list->head != NULL)
        CU_ASSERT_PTR_NULL(list->head->prev);

    if (list->tail != NULL)
        CU_ASSERT_PTR_NULL(list->tail->next);

    list_item* item = list->head;
    for (size_t i = 0; i < n; i++) {
        /* printf("%zu - %d - %d\n", i, *(int*)item->payload,
         * *(int*)expected[i]); */
        if (item == NULL || item->payload == NULL)
            CU_FAIL("null item")
        else{
            CU_ASSERT_PTR_EQUAL(item->payload, expected[i]);
            item = item->next;
        }
    }
    CU_ASSERT_PTR_NULL(item);

    item = list->tail;
    for (size_t i = 1; i <= n; i++) {
        /* printf(
           "%zu - %d - %d\n", i, *(int*)item->payload, *(int*)expected[n -
           i]);*/
        if (item == NULL || item->payload == NULL)
            CU_FAIL("null item")
        else{
            CU_ASSERT_PTR_EQUAL(item->payload, expected[n - i]);
            item = item->prev;
        }
    }
    CU_ASSERT_PTR_NULL(item);
}

/*************************** list_init ****************************************/
static void init_null()
{
    int result = list_init(NULL);
    CU_ASSERT_EQUAL(-1, result);
}

static void init_initalizes_values()
{
    list list;
    list_item dummy;

    // put some values in just make sure the test isn't passing just b/c the
    // list just happens to be all 0 on creation
    list.size = 5;
    list.head = &dummy;
    list.tail = &dummy;

    int result = list_init(&list);

    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_EQUAL(0, list.size);
    CU_ASSERT_PTR_NULL(list.head);
    CU_ASSERT_PTR_NULL(list.tail);
}

/*************************** list_insert_at ***********************************/
static void insert_null()
{
    int result = list_insert_at(NULL, NULL, 0);
    CU_ASSERT_EQUAL(-1, result);
}

static void insert_bad_index()
{
    list list;
    list_init(&list);

    int result = list_insert_at(&list, NULL, 5);

    CU_ASSERT_EQUAL(-1, result);
}

static void insert_into_empty()
{
    int payload = 138;
    void* expected[] = { &payload };
    list list;
    list_init(&list);

    int result = list_insert_at(&list, &payload, 0);

    CU_ASSERT_EQUAL(0, result);
    _list_is_valid(&list, 1, expected);

    list_free(&list);
}

static void insert_into_tail()
{
    int head_payload = 138;
    int tail_payload = 1138;
    void* expected[] = { &head_payload, &tail_payload };
    list list;
    list_init(&list);

    int result = list_insert_at(&list, &head_payload, 0);
    CU_ASSERT_EQUAL(0, result);

    result = list_insert_at(&list, &tail_payload, 1);
    CU_ASSERT_EQUAL(0, result);

    _list_is_valid(&list, 2, expected);

    list_free(&list);
}

static void insert_into_middle()
{
    int head_payload = 138;
    int mid_payload = 1138;
    int tail_payload = 11138;
    void* expected[] = { &head_payload, &mid_payload, &tail_payload };
    list list;
    list_init(&list);

    int result = list_insert_at(&list, &head_payload, 0);
    CU_ASSERT_EQUAL(0, result);

    result = list_insert_at(&list, &tail_payload, 1);
    CU_ASSERT_EQUAL(0, result);

    result = list_insert_at(&list, &mid_payload, 1);
    CU_ASSERT_EQUAL(0, result);

    _list_is_valid(&list, 3, expected);

    list_free(&list);
}

static void insert_head_creates_links()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { &payload[6], &payload[5], &payload[4], &payload[3],
        &payload[2], &payload[1], &payload[0] };
    int result;
    list list;
    list_init(&list);

    for (size_t i = 0; i < n; i++) {
        result = list_insert_at(&list, &payload[i], 0);
        CU_ASSERT_EQUAL(0, result);
    }

    _list_is_valid(&list, n, expected);

    list_free(&list);
}

static void insert_tail_creates_links()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { &payload[0], &payload[1], &payload[2], &payload[3],
        &payload[4], &payload[5], &payload[6] };
    int result;
    list list;
    list_init(&list);

    for (size_t i = 0; i < n; i++) {
        result = list_insert_at(&list, &payload[i], i);
        CU_ASSERT_EQUAL(0, result);
    }

    _list_is_valid(&list, n, expected);

    list_free(&list);
}

static void insert_mid_creates_links()
{
    int payload[] = { 0, 1, 2 };
    void* expected[] = { &payload[0], &payload[2], &payload[1] };
    int result;
    list list;
    list_init(&list);

    result = list_insert_at(&list, &payload[0], 0);
    CU_ASSERT_EQUAL(0, result);

    result = list_insert_at(&list, &payload[1], 1);
    CU_ASSERT_EQUAL(0, result);

    result = list_insert_at(&list, &payload[2], 1);
    CU_ASSERT_EQUAL(0, result);

    _list_is_valid(&list, 3, expected);

    list_free(&list);
}

/*************************** list_free ****************************************/
static void free_null()
{
    int result = list_free(NULL);
    CU_ASSERT_EQUAL(-1, result);
}

static void free_test(void* ptr)
{
    is_free = 1;
    free(ptr);
}

static int free_suite_init()
{
    is_free = 0;
    return 0;
}

static void free_calls_freeme()
{
    int* payload = malloc(sizeof(int));
    list list;
    list_init(&list);

    int result = list_insert_at_free(&list, payload, free_test, 0);

    list_free(&list);
    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_EQUAL(1, is_free);
}

static void free_null_freeme()
{
    int payload = 5;
    list list;
    list_init(&list);

    int result = list_insert_at_free(&list, &payload, NULL, 0);

    list_free(&list);
    CU_ASSERT_EQUAL(0, result);
}

/*************************** list_delete_at ***********************************/
static void delete_null()
{
    int result = list_delete_at(NULL, 0);
    CU_ASSERT_EQUAL(-1, result);
}

static void delete_calls_freeme()
{
    int* payload = malloc(sizeof(int));
    list list;
    list_init(&list);

    list_insert_at_free(&list, payload, free_test, 0);
    int result = list_delete_at(&list, 0);

    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_EQUAL(1, is_free);

    list_free(&list);
}

static void delete_all_items()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { NULL };
    int result;
    list list;
    list_init(&list);

    for (size_t i = 0; i < n; i++)
        list_insert_at(&list, &payload[i], 0);

    for (size_t i = 0; i < n; i++) {
        result = list_delete_at(&list, 0);
        CU_ASSERT_EQUAL(0, result);
    }

    _list_is_valid(&list, 0, expected);

    list_free(&list);
}

static void delete_head_modifies_links()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { &payload[5], &payload[4], &payload[3], &payload[2],
        &payload[1], &payload[0] };
    int result;
    list list;
    list_init(&list);

    for (size_t i = 0; i < n; i++)
        list_insert_at(&list, &payload[i], 0);

    result = list_delete_at(&list, 0);
    CU_ASSERT_EQUAL(0, result);
    _list_is_valid(&list, n - 1, expected);

    list_free(&list);
}

static void delete_tail_modifies_links()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { &payload[6], &payload[5], &payload[4], &payload[3],
        &payload[2], &payload[1] };
    int result;
    list list;
    list_init(&list);

    for (size_t i = 0; i < n; i++)
        list_insert_at(&list, &payload[i], 0);

    result = list_delete_at(&list, n - 1);
    CU_ASSERT_EQUAL(0, result);
    _list_is_valid(&list, n - 1, expected);

    list_free(&list);
}

static void delete_mid_modifies_links()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    void* expected[] = { &payload[6], &payload[5], &payload[3], &payload[2],
        &payload[1], &payload[0] };
    int result;
    list list;
    list_init(&list);

    for (size_t i = 0; i < n; i++)
        list_insert_at(&list, &payload[i], 0);

    result = list_delete_at(&list, 2);
    CU_ASSERT_EQUAL(0, result);
    _list_is_valid(&list, n - 1, expected);

    list_free(&list);
}

static void delete_invalid_index()
{
    list list;
    list_init(&list);

    int result = list_delete_at(&list, 1);

    CU_ASSERT_EQUAL(-1, result);
}

/*************************** list_get_at **************************************/
static void get_null()
{
    void* result = list_get_at(NULL, 0);
    CU_ASSERT_PTR_NULL(result);
}

static void get_retrieves_head()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    list list;
    list_init(&list);

    for (size_t i = 0; i < n; i++)
        list_insert_at(&list, &payload[i], i);

    void* result = list_get_at(&list, 0);
    CU_ASSERT_PTR_EQUAL(&payload[0], result);

    list_free(&list);
}

static void get_retrieves_tail()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    list list;
    list_init(&list);

    for (size_t i = 0; i < n; i++)
        list_insert_at(&list, &payload[i], i);

    void* result = list_get_at(&list, n - 1);
    CU_ASSERT_PTR_EQUAL(&payload[n - 1], result);

    list_free(&list);
}

static void get_retrieves_first_half()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    list list;
    list_init(&list);

    for (size_t i = 0; i < n; i++)
        list_insert_at(&list, &payload[i], i);

    void* result = list_get_at(&list, 2);
    CU_ASSERT_PTR_EQUAL(&payload[2], result);

    list_free(&list);
}

static void get_retrieves_last_half()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    list list;
    list_init(&list);

    for (size_t i = 0; i < n; i++)
        list_insert_at(&list, &payload[i], i);

    void* result = list_get_at(&list, 5);
    CU_ASSERT_PTR_EQUAL(&payload[5], result);

    list_free(&list);
}

static void get_invalid_index()
{
    size_t n = 7;
    int payload[] = { 0, 1, 2, 3, 4, 5, 6 };
    list list;
    list_init(&list);

    for (size_t i = 0; i < n; i++)
        list_insert_at(&list, &payload[i], i);

    void* result = list_get_at(&list, 7);
    CU_ASSERT_PTR_NULL(result);

    list_free(&list);
}

/*************************** array_insert *************************************/
static void array_insert_null()
{
    void* result = array_insert(NULL, 0, 0);
    CU_ASSERT_PTR_NULL(result);
}

static void array_insert_item()
{
    size_t n = 2;
    int arr[] = { 1, 2 };
    int expected[] = { 0, 1, 2 };

    int* result = (int*)array_insert(arr, n, sizeof(arr[0]));
    CU_ASSERT_PTR_NOT_NULL_FATAL(result);

    *result = 0; // NOLINT
    for (size_t i = 0; i < n + 1; i++)
        CU_ASSERT_EQUAL(expected[i], result[i]);
    free(result);
}

static int register_test_suites()
{
    CU_TestInfo init_tests[] = { CU_TEST_INFO(init_null),
        CU_TEST_INFO(init_initalizes_values), CU_TEST_INFO_NULL };

    CU_TestInfo insert_at_tests[] = { CU_TEST_INFO(insert_null),
        CU_TEST_INFO(insert_bad_index), CU_TEST_INFO(insert_into_empty),
        CU_TEST_INFO(insert_into_tail), CU_TEST_INFO(insert_into_middle),
        CU_TEST_INFO(insert_head_creates_links),
        CU_TEST_INFO(insert_mid_creates_links),
        CU_TEST_INFO(insert_tail_creates_links), CU_TEST_INFO_NULL };

    CU_TestInfo free_tests[]
        = { CU_TEST_INFO(free_null), CU_TEST_INFO(free_calls_freeme),
            CU_TEST_INFO(free_null_freeme), CU_TEST_INFO_NULL };

    CU_TestInfo delete_at_tests[]
        = { CU_TEST_INFO(delete_null), CU_TEST_INFO(delete_calls_freeme),
            CU_TEST_INFO(delete_head_modifies_links),
            CU_TEST_INFO(delete_all_items),
            CU_TEST_INFO(delete_tail_modifies_links),
            CU_TEST_INFO(delete_mid_modifies_links),
            CU_TEST_INFO(delete_invalid_index), CU_TEST_INFO_NULL };

    CU_TestInfo get_at_tests[] = { CU_TEST_INFO(get_null),
        CU_TEST_INFO(get_retrieves_head), CU_TEST_INFO(get_retrieves_tail),
        CU_TEST_INFO(get_retrieves_first_half),
        CU_TEST_INFO(get_retrieves_last_half), CU_TEST_INFO(get_invalid_index),
        CU_TEST_INFO_NULL };

    CU_TestInfo array_insert_tests[] = { CU_TEST_INFO(array_insert_null),
        CU_TEST_INFO(array_insert_item), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "list_init suite",
        .pInitFunc = noop,
        .pCleanupFunc = noop,
        .pTests = init_tests },
                 { .pName = "list_insert_at suite",
                     .pInitFunc = noop,
                     .pCleanupFunc = noop,
                     .pTests = insert_at_tests },
                 { .pName = "list_free suite",
                     .pInitFunc = free_suite_init,
                     .pCleanupFunc = noop,
                     .pTests = free_tests },
                 { .pName = "list_delete_at suite",
                     .pInitFunc = free_suite_init,
                     .pCleanupFunc = noop,
                     .pTests = delete_at_tests },
                 { .pName = "list_get_at suite",
                     .pInitFunc = noop,
                     .pCleanupFunc = noop,
                     .pTests = get_at_tests },
                 { .pName = "array_insert suite",
                     .pInitFunc = noop,
                     .pCleanupFunc = noop,
                     .pTests = array_insert_tests },
                 CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}

int main(void)
{
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    if (register_test_suites() != 0) {
        CU_cleanup_registry();
        return -1;
    }

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // You must get this value before CU_cleanup_registry() or it will revert to
    // zero
    int ret = (CU_get_number_of_failure_records() != 0);

    /* Clean up registry and return */
    CU_cleanup_registry();
    return ret;
}
