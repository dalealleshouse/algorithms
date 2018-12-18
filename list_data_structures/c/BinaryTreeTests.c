#include <limits.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "BinaryTree.h"
#include "TreeTestHelpers.h"
#include "include/ErrorReporter.h"
#include "include/TestHelpers.h"

#define SUT(vals, code_block)                                                  \
    {                                                                          \
        BinaryTree* sut = CreateSut(vals);                                     \
        code_block;                                                            \
        BinaryTree_Destroy(sut, NULL);                                         \
    }

static const int TERMINATOR = INT_MIN;
static const size_t mock_n = 13;

static int empty_vals[] = { TERMINATOR };
static int mock_vals[]
    = { 50, 25, 33, 11, 30, 40, 75, 61, 89, 52, 82, 95, 55, TERMINATOR };
static const int mock_ordered[]
    = { 11, 25, 30, 33, 40, 50, 52, 55, 61, 75, 82, 89, 95, TERMINATOR };

static BinaryTree* CreateSut(int* vals)
{
    BinaryTree* tree = BinaryTree_Create(int_comparator);

    while (*vals != TERMINATOR) {
        BinaryTree_Insert(tree, vals);
        vals++;
    }

    return tree;
}

/*************************** BinaryTree_Create ********************************/
static void BinaryTree_Create_null_parmeter()
{
    UNARY_NULL_TEST(ListOp_NullParameter, BinaryTree_Create);
}

static void BinaryTree_Create_failed_mem_allocation()
{
    ErrorReporter_Clear();
    BinaryTree* sut;

    FAILED_MALLOC_TEST({ sut = BinaryTree_Create(int_comparator); })

    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(ListOp_FailedMalloc, ErrorReporter_LastErrorCode());
}

static void BinaryTree_Create_init_values()
{
    BinaryTree* sut = BinaryTree_Create(int_comparator);

    CU_ASSERT_PTR_NOT_NULL(sut);
    CU_ASSERT_PTR_EQUAL(int_comparator, sut->comparator);
    CU_ASSERT_PTR_NULL(sut->root);
    CU_ASSERT_EQUAL(0, sut->n);

    BinaryTree_Destroy(sut, NULL);
}

/*************************** BinaryTree_Insert ********************************/
static void BinaryTree_Insert_failed_mem_allocation()
{
    BinaryTree* sut = BinaryTree_Create(int_comparator);
    int value = 5;
    ListOpResult result;

    FAILED_MALLOC_TEST({ result = BinaryTree_Insert(sut, &value); })

    CU_ASSERT_EQUAL(ListOp_FailedMalloc, result);
    BinaryTree_Destroy(sut, NULL);
}

static void BinaryTree_Insert_creates_root()
{
    BinaryTree* sut = BinaryTree_Create(int_comparator);
    int value = 5;

    ListOpResult result = BinaryTree_Insert(sut, &value);
    CU_ASSERT_EQUAL(ListOp_Success, result);
    CU_ASSERT_PTR_NOT_NULL(sut->root);
    CU_ASSERT_EQUAL(1, sut->n);
    TreeIsValid(sut, 1);

    BinaryTree_Destroy(sut, NULL);
}

static void BinaryTree_Insert_inits_values()
{
    BinaryTree* sut = BinaryTree_Create(int_comparator);
    int value = 5;

    BinaryTree_Insert(sut, &value);

    BinaryTreeNode* node = sut->root;
    CU_ASSERT_PTR_EQUAL(&value, node->payload);
    CU_ASSERT_PTR_NULL(node->left);
    CU_ASSERT_PTR_NULL(node->right);
    CU_ASSERT_PTR_NULL(node->parent);
    CU_ASSERT_EQUAL(1, node->size);

    BinaryTree_Destroy(sut, NULL);
}

static void BinaryTree_Insert_maintains_tree_property()
{
    SUT(mock_vals, { TreePropertyHolds(sut->root); });
}

static void BinaryTree_Insert_sets_size()
{
    SUT(mock_vals, { TreeSizeEqualsNodeCount(sut); });
}

static void BinaryTree_Insert_creates_valid_tree()
{
    SUT(mock_vals, { TreeIsValid(sut, mock_n); });
}

/*************************** BinaryTree_Delete ********************************/
static void BinaryTree_Delete_null_paramter()
{
    SUT(mock_vals, {
        int search_for = 5;
        BINARY_NULL_TEST(
            ListOp_NullParameter, sut, &search_for, BinaryTree_Delete);
    });
}

static void BinaryTree_Delete_empty()
{
    SUT(empty_vals, {
        int search_for = 5;
        ErrorReporter_Clear();
        BinaryTree_Delete(sut, &search_for);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Delete_not_found()
{
    SUT(mock_vals, {
        int not_found = 401;
        ErrorReporter_Clear();
        void* result = BinaryTree_Delete(sut, &not_found);
        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_NotFound, ErrorReporter_LastErrorCode());
        TreeIsValid(sut, mock_n);
    });
}

static void BinaryTree_DeleteTest(int doomed)
{
    SUT(mock_vals, {
        void* result = BinaryTree_Delete(sut, &doomed);
        CU_ASSERT_EQUAL(doomed, *(int*)result);
        TreeIsValid(sut, mock_n - 1);

        ErrorReporter_Clear();
        void* found = BinaryTree_Search(sut, &doomed);
        CU_ASSERT_PTR_NULL(found);
        CU_ASSERT_EQUAL(ListOp_NotFound, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Delete_leaf() { BinaryTree_DeleteTest(11); }

static void BinaryTree_Delete_degree_1_node() { BinaryTree_DeleteTest(52); }

static void BinaryTree_Delete_degree_2_node() { BinaryTree_DeleteTest(75); }

static void BinaryTree_Delete_root() { BinaryTree_DeleteTest(50); }

static void BinaryTree_Delete_last_item()
{
    SUT(mock_vals, {
        for (size_t i = 0; i < mock_n; i++) {
            int doomed = mock_ordered[i];
            void* result = BinaryTree_Delete(sut, &doomed);
            CU_ASSERT_EQUAL(doomed, *(int*)result);
            TreeIsValid(sut, mock_n - i - 1);
        }

        CU_ASSERT_EQUAL(0, sut->n);
        CU_ASSERT_PTR_NULL(sut->root);
    });
}

static void BinaryTree_Delete_standard()
{
    for (size_t i = 0; i < mock_n; i++) {
        SUT(mock_vals, {
            int doomed = mock_ordered[i];
            void* result = BinaryTree_Delete(sut, &doomed);
            CU_ASSERT_EQUAL(doomed, *(int*)result);
            TreeIsValid(sut, mock_n - 1);

            ErrorReporter_Clear();
            void* found = BinaryTree_Search(sut, &doomed);
            CU_ASSERT_PTR_NULL(found);
            CU_ASSERT_EQUAL(ListOp_NotFound, ErrorReporter_LastErrorCode());
        });
    }
}

/*************************** BinaryTree_Enumerate *****************************/
static size_t mock_pos = 0;
static void MockItemHandler(void* item)
{
    CU_ASSERT_EQUAL(mock_ordered[mock_pos], *(int*)item);
    mock_pos++;
}

static void BinaryTree_Enumerate_null_paramter()
{
    SUT(mock_vals, {
        BINARY_INT_NULL_TEST(
            ListOp_NullParameter, sut, MockItemHandler, BinaryTree_Enumerate);
    });
}

static void BinaryTree_Enumerate_empty()
{
    SUT(empty_vals, {
        ListOpResult result = BinaryTree_Enumerate(sut, MockItemHandler);
        CU_ASSERT_EQUAL(ListOp_Success, result);
    });
}

static void BinaryTree_Enumerate_standard()
{
    SUT(mock_vals, {
        mock_pos = 0;
        ListOpResult result = BinaryTree_Enumerate(sut, MockItemHandler);
        CU_ASSERT_EQUAL(ListOp_Success, result);
        CU_ASSERT_EQUAL(mock_n, mock_pos);
    });
}

/*************************** BinaryTree_Search ********************************/
static void BinaryTree_Search_not_found()
{
    SUT(mock_vals, {
        int not_found = 401;
        ErrorReporter_Clear();
        void* result = BinaryTree_Search(sut, &not_found);
        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_NotFound, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Search_empty()
{
    SUT(empty_vals, {
        int search_for = 5;

        ErrorReporter_Clear();
        void* result = BinaryTree_Search(sut, &search_for);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Search_standard()
{
    SUT(mock_vals, {
        int* val = mock_vals;

        while (*val != TERMINATOR) {
            void* result = BinaryTree_Search(sut, val);
            CU_ASSERT_PTR_NOT_NULL(result);
            CU_ASSERT_EQUAL(*(int*)result, *val);
            val++;
        }
    });
}

/*************************** BinaryTree_Min ***********************************/
static void BinaryTree_Min_empty()
{
    SUT(empty_vals, {
        ErrorReporter_Clear();
        void* result = BinaryTree_Min(sut);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Min_standard()
{
    SUT(mock_vals, {
        void* result = BinaryTree_Min(sut);
        CU_ASSERT_EQUAL(11, *(int*)result);
    });
}

/*************************** BinaryTree_Max ***********************************/
static void BinaryTree_Max_empty()
{
    SUT(empty_vals, {
        ErrorReporter_Clear();
        void* result = BinaryTree_Max(sut);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Max_standard()
{
    SUT(mock_vals, {
        void* result = BinaryTree_Max(sut);
        CU_ASSERT_EQUAL(95, *(int*)result);
    });
}

/*************************** BinaryTree_Predecessor ***************************/
static void BinaryTree_Predecessor_empty()
{
    SUT(empty_vals, {
        int search_for = 5;

        ErrorReporter_Clear();
        void* result = BinaryTree_Predecessor(sut, &search_for);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Predecessor_not_found()
{
    SUT(mock_vals, {
        int not_found = 401;

        ErrorReporter_Clear();
        void* result = BinaryTree_Predecessor(sut, &not_found);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_NotFound, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Predecessor_first_item()
{
    SUT(mock_vals, {
        int first = 11;

        ErrorReporter_Clear();
        void* result = BinaryTree_Predecessor(sut, &first);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(0, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Predecessor_standard()
{
    SUT(mock_vals, {
        for (size_t i = 1; i < mock_n; i++) {
            void* result = BinaryTree_Predecessor(sut, &mock_ordered[i]);
            CU_ASSERT_EQUAL(mock_ordered[i - 1], *(int*)result);
        }
    });
}

/*************************** BinaryTree_Successor *****************************/
static void BinaryTree_Successor_empty()
{
    SUT(empty_vals, {
        int search_for = 5;

        ErrorReporter_Clear();
        void* result = BinaryTree_Successor(sut, &search_for);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Successor_not_found()
{
    SUT(mock_vals, {
        int not_found = 401;

        ErrorReporter_Clear();
        void* result = BinaryTree_Successor(sut, &not_found);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_NotFound, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Successor_last_item()
{
    SUT(mock_vals, {
        int last = 95;

        ErrorReporter_Clear();
        void* result = BinaryTree_Successor(sut, &last);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(0, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Successor_standard()
{
    SUT(mock_vals, {
        for (size_t i = 0; i < mock_n - 1; i++) {
            void* result = BinaryTree_Successor(sut, &mock_ordered[i]);
            CU_ASSERT_EQUAL(mock_ordered[i + 1], *(int*)result);
        }
    });
}

/*************************** BinaryTree_Select ********************************/
static void BinaryTree_Select_null_paramter()
{
    ErrorReporter_Clear();
    void* result = BinaryTree_Select(NULL, 1);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(ListOp_NullParameter, ErrorReporter_LastErrorCode());
}

static void BinaryTree_Select_empty()
{
    SUT(empty_vals, {
        ErrorReporter_Clear();
        void* result = BinaryTree_Select(sut, 0);
        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Select_out_of_bounds()
{
    SUT(mock_vals, {
        ErrorReporter_Clear();
        void* result = BinaryTree_Select(sut, mock_n + 1);
        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_InvalidIndex, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Select_standard()
{
    SUT(mock_vals, {
        for (size_t i = 0; i < mock_n; i++) {
            void* result = BinaryTree_Select(sut, i);
            CU_ASSERT_EQUAL(mock_ordered[i], *(int*)result);
        }
    });
}

/*************************** BinaryTree_Rank **********************************/
static void BinaryTree_Rank_null_paramter()
{
    SUT(mock_vals, {
        int search_for = 5;

        ErrorReporter_Clear();
        size_t result = BinaryTree_Rank(NULL, NULL);
        CU_ASSERT_EQUAL(RANK_ERROR, result);
        CU_ASSERT_EQUAL(ListOp_NullParameter, ErrorReporter_LastErrorCode());

        ErrorReporter_Clear();
        result = BinaryTree_Rank(sut, NULL);
        CU_ASSERT_EQUAL(RANK_ERROR, result);
        CU_ASSERT_EQUAL(ListOp_NullParameter, ErrorReporter_LastErrorCode());

        ErrorReporter_Clear();
        result = BinaryTree_Rank(NULL, &search_for);
        CU_ASSERT_EQUAL(RANK_ERROR, result);
        CU_ASSERT_EQUAL(ListOp_NullParameter, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Rank_empty()
{
    SUT(empty_vals, {
        int search_for = 5;

        ErrorReporter_Clear();
        size_t result = BinaryTree_Rank(sut, &search_for);
        CU_ASSERT_EQUAL(RANK_ERROR, result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Rank_not_found()
{
    SUT(mock_vals, {
        int not_found = 401;
        ErrorReporter_Clear();
        size_t result = BinaryTree_Rank(sut, &not_found);
        CU_ASSERT_EQUAL(RANK_ERROR, result);
        CU_ASSERT_EQUAL(ListOp_NotFound, ErrorReporter_LastErrorCode());
    });
}

static void BinaryTree_Rank_standard()
{
    SUT(mock_vals, {
        for (size_t i = 0; i < mock_n; i++) {
            size_t result = BinaryTree_Rank(sut, &mock_ordered[i]);
            CU_ASSERT_EQUAL(result, i);
        }
    });
}

/*************************** BinaryTree_Destory *******************************/
static void BinaryTree_Destroy_null()
{
    // if this does not blow up, it passes
    BinaryTree_Destroy(NULL, NULL);
}

size_t free_tracker = 0;
static void TestFreer(void* val)
{
    (void)val;
    free_tracker++;
}

static void BinaryTree_Destroy_runs_freer()
{
    free_tracker = 0;
    BinaryTree* sut = CreateSut(mock_vals);
    BinaryTree_Destroy(sut, TestFreer);
    CU_ASSERT_EQUAL(mock_n, free_tracker);
}

/*************************** Common *******************************************/
static void BinaryTree_null_parameters()
{
    BinaryTree* tree = BinaryTree_Create(int_comparator);
    int search_for = 5;

    ListOpResult (*binary_int_tests[])(BinaryTree*, void*)
        = { BinaryTree_Insert, NULL };
    BINARY_INT_NULL_TEST_ENUMERATOR(
        ListOp_NullParameter, tree, &search_for, binary_int_tests);

    void* (*binary_tests[])(const BinaryTree*, const void*)
        = { BinaryTree_Search, BinaryTree_Predecessor, BinaryTree_Successor,
              NULL };
    BINARY_NULL_TEST_ENUMERATOR(
        ListOp_NullParameter, tree, &search_for, binary_tests);

    void* (*tests[])(const BinaryTree*)
        = { BinaryTree_Min, BinaryTree_Max, NULL };

    UNARY_NULL_TEST_ENUMERATOR(ListOp_NullParameter, tests);

    BinaryTree_Destroy(tree, NULL);
}

static void BinaryTree_validate_validator()
{
    BinaryTree* sut = ManuallyGenerateValidTree();
    TreeIsValid(sut, 10);
    BinaryTree_Destroy(sut, free);
}

int register_search_tree_tests()
{
    CU_TestInfo create_tests[]
        = { CU_TEST_INFO(BinaryTree_Create_null_parmeter),
              CU_TEST_INFO(BinaryTree_Create_failed_mem_allocation),
              CU_TEST_INFO(BinaryTree_Create_init_values), CU_TEST_INFO_NULL };

    CU_TestInfo insert_tests[] = { CU_TEST_INFO(
                                       BinaryTree_Insert_failed_mem_allocation),
        CU_TEST_INFO(BinaryTree_Insert_creates_root),
        CU_TEST_INFO(BinaryTree_Insert_inits_values),
        CU_TEST_INFO(BinaryTree_Insert_maintains_tree_property),
        CU_TEST_INFO(BinaryTree_Insert_sets_size),
        CU_TEST_INFO(BinaryTree_Insert_creates_valid_tree), CU_TEST_INFO_NULL };

    CU_TestInfo delete_tests[]
        = { CU_TEST_INFO(BinaryTree_Delete_null_paramter),
              CU_TEST_INFO(BinaryTree_Delete_empty),
              CU_TEST_INFO(BinaryTree_Delete_not_found),
              CU_TEST_INFO(BinaryTree_Delete_leaf),
              CU_TEST_INFO(BinaryTree_Delete_degree_1_node),
              CU_TEST_INFO(BinaryTree_Delete_degree_2_node),
              CU_TEST_INFO(BinaryTree_Delete_root),
              CU_TEST_INFO(BinaryTree_Delete_standard),
              CU_TEST_INFO(BinaryTree_Delete_last_item), CU_TEST_INFO_NULL };

    CU_TestInfo enumerate_tests[]
        = { CU_TEST_INFO(BinaryTree_Enumerate_null_paramter),
              CU_TEST_INFO(BinaryTree_Enumerate_empty),
              CU_TEST_INFO(BinaryTree_Enumerate_standard), CU_TEST_INFO_NULL };

    CU_TestInfo search_tests[] = { CU_TEST_INFO(BinaryTree_Search_not_found),
        CU_TEST_INFO(BinaryTree_Search_standard),
        CU_TEST_INFO(BinaryTree_Search_empty), CU_TEST_INFO_NULL };

    CU_TestInfo min_max_tests[] = { CU_TEST_INFO(BinaryTree_Min_empty),
        CU_TEST_INFO(BinaryTree_Min_standard),
        CU_TEST_INFO(BinaryTree_Max_empty),
        CU_TEST_INFO(BinaryTree_Max_standard), CU_TEST_INFO_NULL };

    CU_TestInfo predecessor_tests[] = { CU_TEST_INFO(
                                            BinaryTree_Predecessor_empty),
        CU_TEST_INFO(BinaryTree_Predecessor_not_found),
        CU_TEST_INFO(BinaryTree_Predecessor_first_item),
        CU_TEST_INFO(BinaryTree_Predecessor_standard), CU_TEST_INFO_NULL };

    CU_TestInfo successor_tests[] = { CU_TEST_INFO(BinaryTree_Successor_empty),
        CU_TEST_INFO(BinaryTree_Successor_not_found),
        CU_TEST_INFO(BinaryTree_Successor_last_item),
        CU_TEST_INFO(BinaryTree_Successor_standard), CU_TEST_INFO_NULL };

    CU_TestInfo select_tests[]
        = { CU_TEST_INFO(BinaryTree_Select_null_paramter),
              CU_TEST_INFO(BinaryTree_Select_empty),
              CU_TEST_INFO(BinaryTree_Select_out_of_bounds),
              CU_TEST_INFO(BinaryTree_Select_standard), CU_TEST_INFO_NULL };

    CU_TestInfo rank_tests[] = { CU_TEST_INFO(BinaryTree_Rank_null_paramter),
        CU_TEST_INFO(BinaryTree_Rank_empty),
        CU_TEST_INFO(BinaryTree_Rank_not_found),
        CU_TEST_INFO(BinaryTree_Rank_standard), CU_TEST_INFO_NULL };

    CU_TestInfo destroy_tests[] = { CU_TEST_INFO(BinaryTree_Destroy_null),
        CU_TEST_INFO(BinaryTree_Destroy_runs_freer), CU_TEST_INFO_NULL };

    CU_TestInfo common_tests[] = { CU_TEST_INFO(BinaryTree_null_parameters),
        CU_TEST_INFO(BinaryTree_validate_validator), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "BinaryTree_Create",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = create_tests },
        { .pName = "BinaryTree_Insert",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = insert_tests },
        { .pName = "BinaryTree_Delete",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = delete_tests },
        { .pName = "BinaryTree_Enumerate",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = enumerate_tests },
        { .pName = "BinaryTree_Search",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = search_tests },
        { .pName = "BinaryTree_Min and BinaryTree_Max",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = min_max_tests },
        { .pName = "BinaryTree_Predecessor",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = predecessor_tests },
        { .pName = "BinaryTree_Successor",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = successor_tests },
        { .pName = "BinaryTree_Select",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = select_tests },
        { .pName = "BinaryTree_Rank",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = rank_tests },
        { .pName = "BinaryTree_Destroy",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = destroy_tests },
        { .pName = "BinaryTree Common",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = common_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
