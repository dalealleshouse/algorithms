#include <stdint.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "MemAllocMock.h"
#include "binary_tree.h"

#define INT2VOIDP(i) (void*)(uintptr_t)(i)
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

/*************************** BinaryTree_Create ********************************/
static void BinaryTree_Create_bad_malloc()
{
    MemAllocMock_InterceptMalloc(NULL);
    BinaryTree* result = BinaryTree_Create(int_comparator);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());
    MemAllocMock_ResetMalloc();
}

static void BinaryTree_Create_inits_values()
{
    BinaryTree* result = BinaryTree_Create(int_comparator);

    CU_ASSERT_PTR_NULL(result->root);
    CU_ASSERT_PTR_EQUAL(int_comparator, result->comparator);

    BinaryTree_Destroy(result);
}

static void BinaryTree_Create_null_param()
{
    BinaryTree* result = BinaryTree_Create(NULL);
    CU_ASSERT_PTR_NULL(result);
}

/*************************** BinaryTree_Insert ********************************/
static void BinaryTree_Insert_bad_malloc()
{
    BinaryTree* tree = BinaryTree_Create(int_comparator);

    MemAllocMock_InterceptMalloc(NULL);
    BinaryTreeResult result = BinaryTree_Insert(tree, INT2VOIDP(5));

    CU_ASSERT_EQUAL(BinaryTree_BadMalloc, result);
    CU_ASSERT_EQUAL(1, MemAllocMock_MallocInterceptCount());
    MemAllocMock_ResetMalloc();

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_null_paramter()
{
    BinaryTree* tree = BinaryTree_Create(int_comparator);

    BinaryTreeResult result = BinaryTree_Insert(tree, NULL);

    CU_ASSERT_EQUAL(BinaryTree_NullParamter, result);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_sets_root()
{
    void* expected = INT2VOIDP(5);
    BinaryTree* tree = BinaryTree_Create(int_comparator);

    BinaryTreeResult result = BinaryTree_Insert(tree, expected);

    CU_ASSERT_EQUAL(BinaryTree_Success, result);
    CU_ASSERT_PTR_NOT_NULL(tree->root);
    CU_ASSERT_PTR_NULL(tree->root->left);
    CU_ASSERT_PTR_NULL(tree->root->right);
    CU_ASSERT_EQUAL(expected, tree->root->item);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_sets_left()
{
    void* expected = INT2VOIDP(5);
    BinaryTree* tree = BinaryTree_Create(int_comparator);

    BinaryTreeResult result = BinaryTree_Insert(tree, INT2VOIDP(10));
    CU_ASSERT_EQUAL(BinaryTree_Success, result);

    result = BinaryTree_Insert(tree, expected);
    CU_ASSERT_EQUAL(BinaryTree_Success, result);

    CU_ASSERT_PTR_NOT_NULL(tree->root->left);
    CU_ASSERT_EQUAL(expected, tree->root->left);

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
        CU_TEST_INFO(BinaryTree_Insert_sets_left), CU_TEST_INFO_NULL };

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
