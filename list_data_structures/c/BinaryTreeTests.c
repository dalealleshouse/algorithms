#include <stdint.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "BinaryTree.h"
#include "MemAllocMock.h"

#define INT2VOIDP(i) (void*)(uintptr_t)(i)
#define VOIDP2INT(i) *(int*)(i)
#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int noop(void) { return 0; }

int int_comparator(const void* x, const void* y)
{
    if (x == y)
        return 0;

    int _x = *(int*)x;
    int _y = *(int*)y;

    if (_x == _y)
        return 0;

    if (_x < _y)
        return -1;

    return 1;
}

static void InsertAndCheckSuccess(BinaryTree* tree, void* value)
{
    ListOpResult result = BinaryTree_Insert(tree, value);
    CU_ASSERT_EQUAL(ListOp_Success, result);
}

static BinaryTree* BinaryTree_CreateWithValue(size_t n, int* vals)
{
    BinaryTree* tree = BinaryTree_Create(int_comparator, NULL);

    for (size_t i = 0; i < n; i++)
        InsertAndCheckSuccess(tree, &vals[i]);

    return tree;
}

/*************************** BinaryTree_Create ********************************/
static void BinaryTree_Create_bad_malloc()
{
    MemAllocMock_InterceptMalloc(NULL);
    BinaryTree* result = BinaryTree_Create(int_comparator, NULL);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());
    MemAllocMock_ResetMalloc();
}

static void BinaryTree_Create_inits_values()
{
    BinaryTree* result = BinaryTree_Create(int_comparator, free);

    CU_ASSERT_PTR_NULL(result->root);
    CU_ASSERT_PTR_EQUAL(int_comparator, result->comparator);
    CU_ASSERT_PTR_EQUAL(free, result->freer);

    BinaryTree_Destroy(result);
}

static void BinaryTree_Create_null_param()
{
    BinaryTree* result = BinaryTree_Create(NULL, NULL);
    CU_ASSERT_PTR_NULL(result);

    result = BinaryTree_Create(NULL, free);
    CU_ASSERT_PTR_NULL(result);
}

/*************************** BinaryTree_Insert ********************************/
static void NodeHasValue(int expected, BinaryTreeNode* node)
{
    CU_ASSERT_PTR_NOT_NULL(node);
    CU_ASSERT_EQUAL(expected, VOIDP2INT(node->item));
}

static void BinaryTree_Insert_bad_malloc()
{
    BinaryTree* tree = BinaryTree_Create(int_comparator, NULL);

    MemAllocMock_InterceptMalloc(NULL);
    ListOpResult result = BinaryTree_Insert(tree, INT2VOIDP(5));

    CU_ASSERT_EQUAL(ListOp_FailedMalloc, result);
    CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());
    MemAllocMock_ResetMalloc();

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_null_paramter()
{
    int dummy = 5;
    CU_ASSERT_EQUAL(ListOp_NullParameter, BinaryTree_Insert(NULL, &dummy));

    BinaryTree* tree = BinaryTree_Create(int_comparator, NULL);
    ListOpResult result = BinaryTree_Insert(tree, NULL);
    CU_ASSERT_EQUAL(ListOp_NullParameter, result);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_sets_root()
{
    int vals[] = { 5 };
    BinaryTree* tree = BinaryTree_CreateWithValue(1, vals);

    NodeHasValue(vals[0], tree->root);
    CU_ASSERT_PTR_NULL(tree->root->left);
    CU_ASSERT_PTR_NULL(tree->root->right);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_sets_left()
{
    int vals[] = { 10, 5 };
    BinaryTree* tree = BinaryTree_CreateWithValue(2, vals);

    NodeHasValue(vals[1], tree->root->left);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_sets_right()
{
    int vals[] = { 10, 15 };
    BinaryTree* tree = BinaryTree_CreateWithValue(2, vals);

    NodeHasValue(vals[1], tree->root->right);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_sets_left_and_right()
{
    int vals[] = { 10, 5, 15 };
    BinaryTree* tree = BinaryTree_CreateWithValue(3, vals);

    NodeHasValue(vals[0], tree->root);
    NodeHasValue(vals[1], tree->root->left);
    NodeHasValue(vals[2], tree->root->right);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_standard()
{
    int vals[] = { 50, 40, 30, 45, 60, 70, 55 };
    BinaryTree* tree = BinaryTree_CreateWithValue(7, vals);

    NodeHasValue(vals[0], tree->root);
    NodeHasValue(vals[1], tree->root->left);
    NodeHasValue(vals[2], tree->root->left->left);
    NodeHasValue(vals[3], tree->root->left->right);
    NodeHasValue(vals[4], tree->root->right);
    NodeHasValue(vals[5], tree->root->right->right);
    NodeHasValue(vals[6], tree->root->right->left);

    BinaryTree_Destroy(tree);
}

/*************************** BinaryTree_Delete ********************************/
static void BinaryTree_Delete_null_parameter()
{
    int dummy = 5;
    CU_ASSERT_EQUAL(ListOp_NullParameter, BinaryTree_Delete(NULL, &dummy));

    BinaryTree* tree = BinaryTree_Create(int_comparator, NULL);
    ListOpResult result = BinaryTree_Delete(tree, NULL);
    CU_ASSERT_EQUAL(ListOp_NullParameter, result);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Delete_deletes_root()
{
    int vals[] = { 5 };
    BinaryTree* tree = BinaryTree_CreateWithValue(1, vals);

    ListOpResult result = BinaryTree_Delete(tree, &vals[0]);
    CU_ASSERT_EQUAL(ListOp_Success, result);
    CU_ASSERT_PTR_NULL(tree->root);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Delete_null_root()
{
    int expected = 5;

    BinaryTree* tree = BinaryTree_Create(int_comparator, NULL);
    ListOpResult result = BinaryTree_Delete(tree, &expected);
    CU_ASSERT_EQUAL(ListOp_NotFound, result);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Delete_not_found()
{
    int dummy = 20;
    int vals[] = { 10, 5, 15 };
    BinaryTree* tree = BinaryTree_CreateWithValue(3, vals);

    ListOpResult result = BinaryTree_Delete(tree, &dummy);
    CU_ASSERT_EQUAL(ListOp_NotFound, result);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Delete_deletes_left()
{
    int vals[] = { 10, 5 };
    BinaryTree* tree = BinaryTree_CreateWithValue(2, vals);

    CU_ASSERT_PTR_NOT_NULL(tree->root->left);
    ListOpResult result = BinaryTree_Delete(tree, &vals[1]);

    CU_ASSERT_EQUAL(ListOp_Success, result);
    CU_ASSERT_PTR_NULL(tree->root->left);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Delete_deletes_right()
{
    int vals[] = { 10, 15 };
    BinaryTree* tree = BinaryTree_CreateWithValue(2, vals);

    CU_ASSERT_PTR_NOT_NULL(tree->root->right);
    ListOpResult result = BinaryTree_Delete(tree, &vals[1]);

    CU_ASSERT_EQUAL(ListOp_Success, result);
    CU_ASSERT_PTR_NULL(tree->root->right);

    BinaryTree_Destroy(tree);
}

static int last_free = 0;
static void free_mock(void* ptr) { last_free = VOIDP2INT(ptr); }

static void BinaryTree_Delete_runs_freer()
{
    last_free = 0;

    int value = 10;
    BinaryTree* tree = BinaryTree_Create(int_comparator, free_mock);
    InsertAndCheckSuccess(tree, &value);

    BinaryTree_Delete(tree, &value);
    CU_ASSERT_EQUAL(value, last_free);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Delete_shifts_left_up()
{
    int vals[] = { 50, 40, 30, 45 };
    BinaryTree* tree = BinaryTree_CreateWithValue(4, vals);
    BinaryTree_Delete(tree, &vals[1]);

    NodeHasValue(vals[0], tree->root);
    NodeHasValue(vals[2], tree->root->left);
    NodeHasValue(vals[3], tree->root->left->right);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Delete_shifts_right_up()
{
    int vals[] = { 50, 60, 70, 55 };
    BinaryTree* tree = BinaryTree_CreateWithValue(4, vals);
    BinaryTree_Delete(tree, &vals[1]);

    NodeHasValue(vals[0], tree->root);
    NodeHasValue(vals[3], tree->root->right);
    NodeHasValue(vals[2], tree->root->right->right);

    BinaryTree_Destroy(tree);
}

/*************************** BinaryTree_Destory *******************************/
static void BinaryTree_Destroy_null_paramter()
{
    // if no error, this is cool
    BinaryTree_Destroy(NULL);
}

static int register_test_suites()
{
    CU_TestInfo Create_tests[] = { CU_TEST_INFO(BinaryTree_Create_bad_malloc),
        CU_TEST_INFO(BinaryTree_Create_inits_values),
        CU_TEST_INFO(BinaryTree_Create_null_param), CU_TEST_INFO_NULL };

    CU_TestInfo Insert_tests[] = { CU_TEST_INFO(BinaryTree_Insert_bad_malloc),
        CU_TEST_INFO(BinaryTree_Insert_sets_root),
        CU_TEST_INFO(BinaryTree_Insert_null_paramter),
        CU_TEST_INFO(BinaryTree_Insert_sets_left),
        CU_TEST_INFO(BinaryTree_Insert_sets_right),
        CU_TEST_INFO(BinaryTree_Insert_sets_left_and_right),
        CU_TEST_INFO(BinaryTree_Insert_standard), CU_TEST_INFO_NULL };

    CU_TestInfo Delete_tests[] = { CU_TEST_INFO(
                                       BinaryTree_Delete_null_parameter),
        CU_TEST_INFO(BinaryTree_Delete_null_root),
        CU_TEST_INFO(BinaryTree_Delete_deletes_root),
        CU_TEST_INFO(BinaryTree_Delete_not_found),
        CU_TEST_INFO(BinaryTree_Delete_deletes_left),
        CU_TEST_INFO(BinaryTree_Delete_deletes_right),
        CU_TEST_INFO(BinaryTree_Delete_runs_freer),
        CU_TEST_INFO(BinaryTree_Delete_shifts_left_up),
        CU_TEST_INFO(BinaryTree_Delete_shifts_right_up), CU_TEST_INFO_NULL };

    CU_TestInfo Destroy_tests[]
        = { CU_TEST_INFO(BinaryTree_Destroy_null_paramter), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "BinaryTree_Create",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = Create_tests },
        { .pName = "BinaryTree_Insert",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Insert_tests },
        { .pName = "BinaryTree_Delete",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Delete_tests },
        { .pName = "BinaryTree_Destroy",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Destroy_tests },
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
