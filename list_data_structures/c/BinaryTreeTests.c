#include <limits.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "BinaryTree.h"
#include "include/ErrorReporter.h"
#include "include/TestHelpers.h"

#define SUT(vals, code_block)                                                  \
    {                                                                          \
        BinaryTree* sut = create_sut(vals);                                    \
        code_block;                                                            \
        BinaryTree_Destroy(sut, NULL);                                         \
    }

#define RED_BLACK_SUT(vals, code_block)                                        \
    {                                                                          \
        BinaryTree* sut = create_red_black_sut(vals);                          \
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

static BinaryTree* create_sut(int* vals)
{
    BinaryTree* tree = BinaryTree_Create(int_comparator);

    while (*vals != TERMINATOR) {
        BinaryTree_Insert(tree, vals);
        vals++;
    }

    return tree;
}

static BinaryTree* create_red_black_sut(int* vals)
{
    BinaryTree* tree = BinaryTree_Create(int_comparator);

    while (*vals != TERMINATOR) {
        RedBlackTree_Insert(tree, vals);
        vals++;
    }

    return tree;
}

static size_t node_count(BinaryTreeNode* node)
{
    if (node == &sentinel)
        return 0;

    size_t nc = node_count(node->left) + node_count(node->right) + 1;
    if (nc != node->size) {
        int node_id = *(int*)node->payload;
        printf("invalid size: id=%d expected=%zu, actual=%zu\n", node_id, nc,
            node->size);
        CU_FAIL();
    }
    return nc;
}

static void size_equals_node_count(BinaryTree* tree)
{
    CU_ASSERT_EQUAL(tree->n, node_count(tree->root));
}

static void node_is_valid(BinaryTreeNode* node)
{
    if (node == &sentinel)
        return;

    int node_id = *(int*)node->payload;

    if (node->left != &sentinel) {
        int result = int_comparator(node->payload, node->left->payload);
        if (result < 0) {
            printf(
                "ASSERT FAILURE on node %d - left item is greater than current",
                node_id);
            CU_FAIL();
        }
    }

    if (node->right != &sentinel) {
        int result = int_comparator(node->payload, node->right->payload);
        if (result > 0) {
            printf(
                "ASSERT FAILURE on node %d - right item is less than current",
                node_id);
            CU_FAIL();
        }
    }
}

static void tree_property_holds(BinaryTreeNode* node)
{
    if (node == &sentinel)
        return;

    tree_property_holds(node->left);
    node_is_valid(node);
    tree_property_holds(node->right);
}

static void tree_is_valid(BinaryTree* tree, size_t n)
{
    CU_ASSERT_PTR_NOT_NULL(tree);

    if (n == 0) {
        CU_ASSERT_PTR_EQUAL(&sentinel, tree->root);
    } else {
        CU_ASSERT_EQUAL(tree->n, tree->root->size);
    }

    CU_ASSERT_EQUAL(n, tree->n);
    size_equals_node_count(tree);
    tree_property_holds(tree->root);
}

static BinaryTree* generate_valid_tree()
{
    const size_t n = 10;
    BinaryTree* sut = BinaryTree_Create(int_comparator);
    BinaryTreeNode* nodes[n];
    for (size_t i = 0; i < n; i++) {
        size_t* val = malloc(sizeof(size_t));
        *val = i;

        nodes[i] = calloc(sizeof(BinaryTreeNode), 1);
        nodes[i]->payload = val;
    }

    nodes[0]->parent = nodes[1];
    nodes[0]->left = &sentinel;
    nodes[0]->right = &sentinel;
    nodes[0]->size = 1;

    nodes[1]->parent = nodes[2];
    nodes[1]->left = nodes[0];
    nodes[1]->right = nodes[3];
    nodes[1]->size = 4;

    nodes[2]->parent = &sentinel;
    nodes[2]->left = nodes[1];
    nodes[2]->right = nodes[7];
    nodes[2]->size = 10;

    nodes[3]->parent = nodes[1];
    nodes[3]->left = &sentinel;
    nodes[3]->right = nodes[4];
    nodes[3]->size = 2;

    nodes[4]->parent = nodes[3];
    nodes[4]->left = &sentinel;
    nodes[4]->right = &sentinel;
    nodes[4]->size = 1;

    nodes[5]->parent = nodes[6];
    nodes[5]->left = &sentinel;
    nodes[5]->right = &sentinel;
    nodes[5]->size = 1;

    nodes[6]->parent = nodes[7];
    nodes[6]->left = nodes[5];
    nodes[6]->right = &sentinel;
    nodes[6]->size = 2;

    nodes[7]->parent = nodes[2];
    nodes[7]->left = nodes[6];
    nodes[7]->right = nodes[8];
    nodes[7]->size = 5;

    nodes[8]->parent = nodes[7];
    nodes[8]->left = &sentinel;
    nodes[8]->right = nodes[9];
    nodes[8]->size = 2;

    nodes[9]->parent = nodes[8];
    nodes[9]->left = &sentinel;
    nodes[9]->right = &sentinel;
    nodes[9]->size = 1;

    sut->root = nodes[2];
    sut->n = n;
    return sut;
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
    CU_ASSERT_PTR_EQUAL(&sentinel, sut->root);
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
    CU_ASSERT_PTR_NOT_EQUAL(&sentinel, sut->root);
    CU_ASSERT_EQUAL(1, sut->n);
    tree_is_valid(sut, 1);

    BinaryTree_Destroy(sut, NULL);
}

static void BinaryTree_Insert_inits_values()
{
    BinaryTree* sut = BinaryTree_Create(int_comparator);
    int value = 5;

    BinaryTree_Insert(sut, &value);

    BinaryTreeNode* node = sut->root;
    CU_ASSERT_PTR_EQUAL(&value, node->payload);
    CU_ASSERT_PTR_EQUAL(&sentinel, node->left);
    CU_ASSERT_PTR_EQUAL(&sentinel, node->right);
    CU_ASSERT_PTR_EQUAL(&sentinel, node->parent);
    CU_ASSERT_EQUAL(1, node->size);

    BinaryTree_Destroy(sut, NULL);
}

static void BinaryTree_Insert_maintains_tree_property()
{
    SUT(mock_vals, { tree_property_holds(sut->root); });
}

static void BinaryTree_Insert_sets_size()
{
    SUT(mock_vals, { size_equals_node_count(sut); });
}

static void BinaryTree_Insert_creates_valid_tree()
{
    SUT(mock_vals, { tree_is_valid(sut, mock_n); });
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
        tree_is_valid(sut, mock_n);
    });
}

static void BinaryTree_DeleteTest(int doomed)
{
    SUT(mock_vals, {
        void* result = BinaryTree_Delete(sut, &doomed);
        CU_ASSERT_EQUAL(doomed, *(int*)result);
        tree_is_valid(sut, mock_n - 1);

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
            tree_is_valid(sut, mock_n - i - 1);
        }

        CU_ASSERT_EQUAL(0, sut->n);
        CU_ASSERT_PTR_EQUAL(&sentinel, sut->root);
    });
}

static void BinaryTree_Delete_standard()
{
    for (size_t i = 0; i < mock_n; i++) {
        SUT(mock_vals, {
            int doomed = mock_ordered[i];
            void* result = BinaryTree_Delete(sut, &doomed);
            CU_ASSERT_EQUAL(doomed, *(int*)result);
            tree_is_valid(sut, mock_n - 1);

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
    BinaryTree* sut = create_sut(mock_vals);
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
    BinaryTree* sut = generate_valid_tree();
    tree_is_valid(sut, 10);
    BinaryTree_Destroy(sut, free);
}

/*************************** RedBlackTree_Insert ******************************/
// Every node is designated as either red or black
static void red_black_invaiant_1(BinaryTreeNode* node)
{
    if (node == NULL)
        return;

    red_black_invaiant_1(node->left);
    CU_ASSERT_TRUE(node->color == RED || node->color == BLACK);
    red_black_invaiant_1(node->right);
}

// Root node is black
static void red_black_invaiant_2(BinaryTree* tree)
{
    CU_ASSERT_TRUE(tree->root->color == BLACK);
}

// No consecutive red nodes
static void red_black_invaiant_3(BinaryTreeNode* node)
{
    if (node == NULL)
        return;

    red_black_invaiant_3(node->left);
    if (node->color == RED) {
        if (node->parent != NULL)
            CU_ASSERT_TRUE(node->parent->color == BLACK);

        if (node->left != NULL)
            CU_ASSERT_TRUE(node->left->color == BLACK);

        if (node->right != NULL)
            CU_ASSERT_TRUE(node->right->color == BLACK);
    }
    red_black_invaiant_3(node->right);
}

// Every path starting at the root node and ending in a NULL pointer must pass
// through the same number of black nodes
static void red_black_invaiant_4(BinaryTree* tree) {}

static void red_black_tree_is_valid(BinaryTree* tree, size_t n)
{
    tree_is_valid(tree, n);
    red_black_invaiant_1(tree->root);
    red_black_invaiant_2(tree);
    red_black_invaiant_3(tree->root);
    red_black_invaiant_4(tree);
}

static void RedBlackTree_Insert_failed_mem_allocation()
{
    BinaryTree* sut = BinaryTree_Create(int_comparator);
    int value = 5;
    ListOpResult result;

    FAILED_MALLOC_TEST({ result = RedBlackTree_Insert(sut, &value); })

    CU_ASSERT_EQUAL(ListOp_FailedMalloc, result);
    BinaryTree_Destroy(sut, NULL);
}

static void RedBlackTree_Insert_creates_root()
{
    int vals[] = { 5, TERMINATOR };
    RED_BLACK_SUT(vals, {
        CU_ASSERT_PTR_NOT_NULL(sut->root);
        red_black_tree_is_valid(sut, 1);
    });
}

static void RedBlackTree_Insert_sets_first_child_red()
{
    int vals[] = { 5, 10, 2, TERMINATOR };
    RED_BLACK_SUT(vals, {
        CU_ASSERT_PTR_NOT_NULL(sut->root);
        CU_ASSERT_EQUAL(RED, sut->root->right->color);
        red_black_tree_is_valid(sut, 3);
    });
}

static void RedBlackTree_Insert_sets_size() { CU_FAIL(); }

static void RedBlackTree_Insert_creates_valid_tree() { CU_FAIL(); }

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

    CU_TestInfo red_black_insert_tests[]
        = { CU_TEST_INFO(RedBlackTree_Insert_failed_mem_allocation),
              CU_TEST_INFO(RedBlackTree_Insert_creates_root),
              CU_TEST_INFO(RedBlackTree_Insert_sets_first_child_red),
              CU_TEST_INFO_NULL };

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
        { .pName = "RedBlackTree_Insert",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = red_black_insert_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
