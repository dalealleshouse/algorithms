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

static void* BuildAndInsert(Structure st)
{
    void* ds = BuildEmptyDataStructure(st);

    InsertOp op = GetInsertOperation(st);
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
    Structure strs[] = { ARRAY, LINKED_LIST, BINARY_TREE };
    checker checkers[]
        = { Array_Checker, LinkedList_Checker, BinaryTree_Checker };

    for (int i = 0; i < 3; i++) {
        void* ds = BuildAndInsert(strs[i]);
        checkers[i](ds);
        DestroyStructure(strs[i], ds);
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

int register_algo_timer_tests()
{
    CU_TestInfo create_tests[] = { CU_TEST_INFO(InsertOp_insert_int_as_pointer),
        CU_TEST_INFO(BuildDataStructure_inserts_correct_number_of_items),
        CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "algo timer",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = create_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
