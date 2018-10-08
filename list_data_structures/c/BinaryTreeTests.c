#include <stdbool.h>
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

typedef struct {
    bool valid;
    size_t n;
} TreeValidationResult;

static int noop(void) { return 0; }

static void InsertAndCheckSuccess(BinaryTree* tree, void* value)
{
    ListOpResult result = BinaryTree_Insert(tree, value);
    CU_ASSERT_EQUAL(ListOp_Success, result);
}

static void ValidateNode(BinaryTreeNode* node, TreeValidationResult* result)
{
    if (node == NULL)
        return;

    result->n++;

    BinaryTreeNode* left = node->left;
    BinaryTreeNode* right = node->right;

    if (left != NULL && int_comparator(left->item, node->item) > 0)
        result->valid = false;

    if (right != NULL && int_comparator(right->item, node->item) <= 0)
        result->valid = false;

    ValidateNode(left, result);
    ValidateNode(right, result);
}

static void ValidateTree(BinaryTree* tree, size_t n)
{
    TreeValidationResult result = { true, 0 };
    ValidateNode(tree->root, &result);
    CU_ASSERT_EQUAL(true, result.valid);
    CU_ASSERT_EQUAL(n, result.n);
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

    ValidateTree(tree, 1);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_sets_left()
{
    int vals[] = { 10, 5 };
    BinaryTree* tree = BinaryTree_CreateWithValue(2, vals);

    ValidateTree(tree, 2);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_sets_right()
{
    int vals[] = { 10, 15 };
    BinaryTree* tree = BinaryTree_CreateWithValue(2, vals);

    ValidateTree(tree, 2);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_sets_left_and_right()
{
    size_t n = 3;
    int vals[] = { 10, 5, 15 };
    BinaryTree* tree = BinaryTree_CreateWithValue(n, vals);

    ValidateTree(tree, n);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Insert_standard()
{
    size_t n = 7;
    int vals[] = { 50, 40, 30, 45, 60, 70, 55 };
    BinaryTree* tree = BinaryTree_CreateWithValue(n, vals);

    ValidateTree(tree, n);

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
    ValidateTree(tree, 0);

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

static void BinaryTree_Delete_deletes_leaf()
{
    size_t n = 15;
    int vals[] = { 50, 25, 10, 33, 4, 11, 30, 40, 75, 56, 89, 52, 61, 82, 95 };
    BinaryTree* tree = BinaryTree_CreateWithValue(n, vals);

    ListOpResult result = BinaryTree_Delete(tree, &vals[4]);

    CU_ASSERT_EQUAL(ListOp_Success, result);
    ValidateTree(tree, n - 1);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Delete_deletes_with_one_child()
{
    size_t n = 14;
    int vals[] = { 50, 25, 10, 33, 11, 30, 40, 75, 56, 89, 52, 61, 82, 95 };
    BinaryTree* tree = BinaryTree_CreateWithValue(n, vals);

    ListOpResult result = BinaryTree_Delete(tree, &vals[2]);

    CU_ASSERT_EQUAL(ListOp_Success, result);
    ValidateTree(tree, n - 1);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Delete_deletes_with_two_children()
{
    size_t n = 13;
    int vals[] = { 50, 25, 33, 11, 30, 40, 75, 56, 89, 52, 61, 82, 95 };
    BinaryTree* tree = BinaryTree_CreateWithValue(n, vals);
    BinaryTree_Delete(tree, &vals[7]);

    ValidateTree(tree, n - 1);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Delete_deletes_root_with_children()
{
    size_t n = 13;
    int vals[] = { 50, 25, 33, 11, 30, 40, 75, 56, 89, 52, 61, 82, 95 };
    BinaryTree* tree = BinaryTree_CreateWithValue(n, vals);
    BinaryTree_Delete(tree, &vals[0]);

    ValidateTree(tree, n - 1);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Delete_deletes_root_with_dangling_children()
{
    size_t n = 13;
    int vals[] = { 50, 25, 33, 11, 30, 40, 75, 61, 89, 52, 82, 95, 55 };
    BinaryTree* tree = BinaryTree_CreateWithValue(n, vals);
    BinaryTree_Delete(tree, &vals[0]);

    ValidateTree(tree, n - 1);

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

/*************************** BinaryTree_Search ********************************/
static void BinaryTree_Search_null_parameter()
{
    BinaryTree* tree = BinaryTree_Create(int_comparator, NULL);

    void* result = BinaryTree_Search(tree, NULL);
    CU_ASSERT_PTR_NULL(result);

    int dummy = 5;
    result = BinaryTree_Search(NULL, &dummy);
    CU_ASSERT_PTR_NULL(result);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Search_not_found()
{
    size_t n = 13;
    int not_found = 401;
    int vals[] = { 50, 25, 33, 11, 30, 40, 75, 61, 89, 52, 82, 95, 55 };
    BinaryTree* tree = BinaryTree_CreateWithValue(n, vals);

    void* result = BinaryTree_Search(tree, &not_found);
    CU_ASSERT_PTR_NULL(result);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Search_standard()
{
    size_t n = 13;
    int search = 25;
    int vals[] = { 50, 25, 33, 11, 30, 40, 75, 61, 89, 52, 82, 95, 55 };
    BinaryTree* tree = BinaryTree_CreateWithValue(n, vals);

    void* result = BinaryTree_Search(tree, &search);
    CU_ASSERT_EQUAL(search, VOIDP2INT(result));

    BinaryTree_Destroy(tree);
}

/*************************** BinaryTree_Enumerate *****************************/
static const size_t mock_n = 13;
static int mock_vals[] = { 50, 25, 33, 11, 30, 40, 75, 61, 89, 52, 82, 95, 55 };
static const int mock_expected[]
    = { 11, 25, 30, 33, 40, 50, 52, 55, 61, 75, 82, 89, 95 };
static int mock_pos = 0;
static void MockItemHandler(void* item)
{
    CU_ASSERT_EQUAL(mock_expected[mock_pos], VOIDP2INT(item));
    mock_pos++;
}

static void BinaryTree_Enumerate_null_paramter()
{
    BinaryTree* tree = BinaryTree_CreateWithValue(mock_n, mock_vals);

    ListOpResult result = BinaryTree_Enumerate(tree, NULL);
    CU_ASSERT_EQUAL(ListOp_NullParameter, result);

    result = BinaryTree_Enumerate(NULL, MockItemHandler);
    CU_ASSERT_EQUAL(ListOp_NullParameter, result);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Enumerate_empty()
{
    BinaryTree* tree = BinaryTree_Create(int_comparator, NULL);

    ListOpResult result = BinaryTree_Enumerate(tree, MockItemHandler);
    CU_ASSERT_EQUAL(ListOp_Success, result);

    BinaryTree_Destroy(tree);
}

static void BinaryTree_Enumerate_standard()
{
    size_t n = 13;
    int vals[] = { 50, 25, 33, 11, 30, 40, 75, 61, 89, 52, 82, 95, 55 };
    BinaryTree* tree = BinaryTree_CreateWithValue(n, vals);

    mock_pos = 0;
    ListOpResult result = BinaryTree_Enumerate(tree, MockItemHandler);
    CU_ASSERT_EQUAL(ListOp_Success, result);
    CU_ASSERT_EQUAL(mock_n, mock_pos);

    BinaryTree_Destroy(tree);
}

/*************************** BinaryTree_Destory *******************************/
static void BinaryTree_Destroy_null_paramter()
{
    // if no error, this is cool
    BinaryTree_Destroy(NULL);
}

static void BinaryTree_Destroy_frees_all_nodes()
{
    // If there is a problem, this will only fail when running address-san
    BinaryTree* tree = BinaryTree_Create(int_comparator, free);

    for (size_t i = 0; i < 50; i++) {
        int* val = malloc(sizeof(int));
        InsertAndCheckSuccess(tree, val);
    }

    BinaryTree_Destroy(tree);
}

int register_binary_tree_tests()
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
        CU_TEST_INFO(BinaryTree_Delete_deletes_leaf),
        CU_TEST_INFO(BinaryTree_Delete_deletes_with_one_child),
        CU_TEST_INFO(BinaryTree_Delete_deletes_with_two_children),
        CU_TEST_INFO(BinaryTree_Delete_deletes_root_with_children),
        CU_TEST_INFO(BinaryTree_Delete_deletes_root_with_dangling_children),
        CU_TEST_INFO(BinaryTree_Delete_runs_freer), CU_TEST_INFO_NULL };

    CU_TestInfo Search_tests[]
        = { CU_TEST_INFO(BinaryTree_Search_null_parameter),
              CU_TEST_INFO(BinaryTree_Search_not_found),
              CU_TEST_INFO(BinaryTree_Search_standard), CU_TEST_INFO_NULL };

    CU_TestInfo Enumerate_tests[]
        = { CU_TEST_INFO(BinaryTree_Enumerate_null_paramter),
              CU_TEST_INFO(BinaryTree_Enumerate_empty),
              CU_TEST_INFO(BinaryTree_Enumerate_standard), CU_TEST_INFO_NULL };

    CU_TestInfo Destroy_tests[] = { CU_TEST_INFO(
                                        BinaryTree_Destroy_null_paramter),
        CU_TEST_INFO(BinaryTree_Destroy_frees_all_nodes), CU_TEST_INFO_NULL };

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
        { .pName = "BinaryTree_Search",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Search_tests },
        { .pName = "BinaryTree_Enumerate",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Enumerate_tests },
        { .pName = "BinaryTree_Destroy",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = Destroy_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
