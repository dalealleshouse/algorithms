#include <stdbool.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "algo_timer.h"

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int noop(void) { return 0; }
static const uintptr_t expected = 5;
static const uintptr_t expected_2 = 10;

const Structure base_structures[] = { ARRAY, LINKED_LIST, BINARY_TREE };
const size_t base_structure_count = 3;

static void* BuildAndInsert(Structure st)
{
    void* ds = BuildEmptyDataStructure(st);

    ListOp op = GetInsertOperation(st);
    op(ds, expected);
    op(ds, expected_2);

    return ds;
}

typedef void (*checker)(void* ds);

static void Array_Checker(void* ds)
{
    Array* array = (Array*)ds;
    uintptr_t actual = (uintptr_t)((uintptr_t*)array->array)[0];
    uintptr_t actual_2 = (uintptr_t)((uintptr_t*)array->array)[1];

    CU_ASSERT_EQUAL(expected, actual);
    CU_ASSERT_EQUAL(expected_2, actual_2);
}

static void LinkedList_Checker(void* ds)
{
    LinkedList* list = (LinkedList*)ds;
    uintptr_t actual = (uintptr_t)list->head->payload;
    uintptr_t actual_2 = (uintptr_t)list->head->next->payload;

    CU_ASSERT_EQUAL(expected, actual);
    CU_ASSERT_EQUAL(expected_2, actual_2);
}

static void BinaryTree_Checker(void* ds)
{
    BinaryTree* tree = (BinaryTree*)ds;

    uintptr_t actual = (uintptr_t)tree->root->item;
    uintptr_t actual_2 = (uintptr_t)tree->root->right->item;

    CU_ASSERT_EQUAL(expected, actual);
    CU_ASSERT_EQUAL(expected_2, actual_2);
}

static void InsertOp_insert_int_as_pointer()
{
    checker checkers[]
        = { Array_Checker, LinkedList_Checker, BinaryTree_Checker };

    for (size_t i = 0; i < base_structure_count; i++) {
        void* ds = BuildAndInsert(base_structures[i]);
        checkers[i](ds);
        DestroyStructure(base_structures[i], ds);
    }
}

typedef size_t (*sizer)(void*);
static size_t ArraySize(void* array) { return ((Array*)array)->n; }
static size_t LinkedListSize(void* list) { return ((LinkedList*)list)->size; }
static size_t BinaryTreeSize(void* tree) { return ((BinaryTree*)tree)->n; }

static void BuildDataStructure_inserts_correct_number_of_items()
{
    size_t n = 10;
    sizer sizers[] = { NULL, ArraySize, LinkedListSize, LinkedListSize,
        BinaryTreeSize, BinaryTreeSize };

    for (int i = 1; i <= BINARY_TREE_UNBALANCED; i++) {
        void* ds = BuildDataStructure(i, n);
        CU_ASSERT_EQUAL(n, sizers[i](ds));
        DestroyStructure(i, ds);
    }
}

static void SearchOp_finds_item()
{
    for (size_t i = 0; i < base_structure_count; i++) {
        void* ds = BuildAndInsert(base_structures[i]);
        SearchOp op = GetSearchOperation(base_structures[i]);

        uintptr_t result = op(ds, expected);
        uintptr_t result_2 = op(ds, expected_2);
        uintptr_t result_3 = op(ds, 11);

        CU_ASSERT_EQUAL(expected, result);
        CU_ASSERT_EQUAL(expected_2, result_2);
        CU_ASSERT_EQUAL(0, result_3);
        DestroyStructure(base_structures[i], ds);
    }
}

static void SearchOp_returns_0_for_not_found()
{
    const uintptr_t not_found = 11;

    for (size_t i = 0; i < base_structure_count; i++) {
        void* ds = BuildAndInsert(base_structures[i]);
        SearchOp op = GetSearchOperation(base_structures[i]);

        uintptr_t result = op(ds, not_found);

        CU_ASSERT_EQUAL(0, result);
        DestroyStructure(base_structures[i], ds);
    }
}

static void EnumerateOp_runs_handler()
{

    for (size_t i = 0; i < base_structure_count; i++) {
        _enumerate_sum = 0;
        void* ds = BuildAndInsert(base_structures[i]);
        EnumerateOp op = GetEnumerateOperation(base_structures[i]);
        op(ds);

        CU_ASSERT_EQUAL(15, _enumerate_sum);
        DestroyStructure(base_structures[i], ds);
    }
}

static void OperationTime_returns_positive_number()
{
    const size_t n = 1000;

    for (size_t i = 1; i <= ENUMERATE; i++) {
        for (size_t j = 1; j <= BINARY_TREE_UNBALANCED; j++) {
            double result = OperationTime(i, j, n);
            CU_ASSERT(result >= 0);
        }
    }
}

int register_algo_timer_tests()
{
    CU_TestInfo create_tests[] = { CU_TEST_INFO(InsertOp_insert_int_as_pointer),
        CU_TEST_INFO(BuildDataStructure_inserts_correct_number_of_items),
        CU_TEST_INFO(SearchOp_finds_item),
        CU_TEST_INFO(SearchOp_returns_0_for_not_found),
        CU_TEST_INFO(EnumerateOp_runs_handler),
        CU_TEST_INFO(OperationTime_returns_positive_number),
        CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "algo timer",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = create_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
