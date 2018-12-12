#include <limits.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "SearchTree.h"
#include "include/ErrorReporter.h"
#include "include/TestHelpers.h"

#define SUT(code_block)                                                        \
    {                                                                          \
        SearchTree* sut = CreateSut(mock_vals);                                \
        code_block;                                                            \
        SearchTree_Destroy(sut, NULL);                                         \
    }

#define EMPTY_SUT(code_block)                                                  \
    {                                                                          \
        int val[] = { TERMINATOR };                                            \
        SearchTree* sut = CreateSut(val);                                      \
        code_block;                                                            \
        SearchTree_Destroy(sut, NULL);                                         \
    }

/*************************** Helpers ******************************************/
const int TERMINATOR = INT_MIN;

static const int mock_n = 13;
static int mock_vals[]
    = { 50, 25, 33, 11, 30, 40, 75, 61, 89, 52, 82, 95, 55, TERMINATOR };

static const int mock_ordered[]
    = { 11, 25, 30, 33, 40, 50, 52, 55, 61, 75, 82, 89, 95 };

static SearchTree* CreateSut(int* vals)
{
    SearchTree* tree = SearchTree_Create(int_comparator);

    while (*vals != TERMINATOR) {
        SearchTree_Insert(tree, vals);
        vals++;
    }

    return tree;
}

static size_t NodeCount(SearchTreeNode* node)
{
    if (node == NULL)
        return 0;

    size_t nc = NodeCount(node->left) + NodeCount(node->right) + 1;
    if (nc != node->size) {
        int node_id = *(int*)node->item;
        printf("invalid size: id=%d expected=%zu, actual=%zu\n", node_id, nc,
            node->size);
        CU_FAIL();
    }
    return nc;
}

static void TreeSizeEqualsNodeCount(SearchTree* tree)
{
    CU_ASSERT_EQUAL(tree->n, NodeCount(tree->root));
}

#ifdef BALANCE
static size_t abs_size_t(size_t x, size_t y)
{
    if (x == y)
        return 0;

    if (x > y)
        return x - y;

    return y - x;
}

static void TreeIsBalanced(SearchTreeNode* node)
{
    if (node == NULL)
        return;

    TreeIsBalanced(node->left);

    int node_id = *(int*)node->item;
    size_t left_size = (node->left == NULL) ? 0 : node->left->size;
    size_t right_size = (node->right == NULL) ? 0 : node->right->size;
    size_t diff = abs_size_t(left_size, right_size);
    if (diff > 1) {
        printf("ASSERT FAILURE on node %d - unbalanced left=%zu, right=%zu\n",
            node_id, left_size, right_size);
        CU_FAIL();
    }

    TreeIsBalanced(node->right);
}
#endif

static void NodeIsValid(SearchTreeNode* node)
{
    if (node == NULL)
        return;

    int node_id = *(int*)node->item;

    if (node->left != NULL) {
        int result = int_comparator(node->item, node->left->item);
        if (result < 0) {
            printf(
                "ASSERT FAILURE on node %d - left item is greater than current",
                node_id);
            CU_FAIL();
        }
    }

    if (node->right != NULL) {
        int result = int_comparator(node->item, node->right->item);
        if (result > 0) {
            printf(
                "ASSERT FAILURE on node %d - right item is less than current",
                node_id);
            CU_FAIL();
        }
    }
}

static void TreePropertyHolds(SearchTreeNode* node)
{
    if (node == NULL)
        return;

    TreePropertyHolds(node->left);
    NodeIsValid(node);
    TreePropertyHolds(node->right);
}

static void TreeIsValid(SearchTree* tree, size_t n)
{
    CU_ASSERT_PTR_NOT_NULL(tree);
    CU_ASSERT_EQUAL(n, tree->n);
    CU_ASSERT_EQUAL(tree->n, tree->root->size);
    TreeSizeEqualsNodeCount(tree);
    TreePropertyHolds(tree->root);
#ifdef BALANCE
    TreeIsBalanced(tree->root);
#endif
}

static SearchTree* ManuallyGenerateValidTree()
{
    const size_t n = 10;
    SearchTree* sut = SearchTree_Create(int_comparator);
    SearchTreeNode* nodes[n];
    for (size_t i = 0; i < n; i++) {
        size_t* val = malloc(sizeof(size_t));
        *val = i;

        nodes[i] = calloc(sizeof(SearchTreeNode), 1);
        nodes[i]->item = val;
    }

    nodes[0]->parent = nodes[1];
    nodes[0]->left = NULL;
    nodes[0]->right = NULL;
    nodes[0]->size = 1;

    nodes[1]->parent = nodes[2];
    nodes[1]->left = nodes[0];
    nodes[1]->right = nodes[3];
    nodes[1]->size = 4;

    nodes[2]->parent = NULL;
    nodes[2]->left = nodes[1];
    nodes[2]->right = nodes[7];
    nodes[2]->size = 10;

    nodes[3]->parent = nodes[1];
    nodes[3]->left = NULL;
    nodes[3]->right = nodes[4];
    nodes[3]->size = 2;

    nodes[4]->parent = nodes[3];
    nodes[4]->left = NULL;
    nodes[4]->right = NULL;
    nodes[4]->size = 1;

    nodes[5]->parent = nodes[6];
    nodes[5]->left = NULL;
    nodes[5]->right = NULL;
    nodes[5]->size = 1;

    nodes[6]->parent = nodes[7];
    nodes[6]->left = nodes[5];
    nodes[6]->right = NULL;
    nodes[6]->size = 2;

    nodes[7]->parent = nodes[2];
    nodes[7]->left = nodes[6];
    nodes[7]->right = nodes[8];
    nodes[7]->size = 5;

    nodes[8]->parent = nodes[7];
    nodes[8]->left = NULL;
    nodes[8]->right = nodes[9];
    nodes[8]->size = 2;

    nodes[9]->parent = nodes[8];
    nodes[9]->left = NULL;
    nodes[9]->right = NULL;
    nodes[9]->size = 1;

    sut->root = nodes[2];
    sut->n = n;
    return sut;
}

/*************************** SearchTree_Create ********************************/
static void SearchTree_Create_null_parmeter()
{
    UNARY_NULL_TEST(ListOp_NullParameter, SearchTree_Create);
}

static void SearchTree_Create_failed_mem_allocation()
{
    ErrorReporter_Clear();
    SearchTree* sut;

    FAILED_MALLOC_TEST({ sut = SearchTree_Create(int_comparator); })

    CU_ASSERT_PTR_NULL(sut);
    CU_ASSERT_EQUAL(ListOp_FailedMalloc, ErrorReporter_LastErrorCode());
}

static void SearchTree_Create_init_values()
{
    SearchTree* sut = SearchTree_Create(int_comparator);

    CU_ASSERT_PTR_NOT_NULL(sut);
    CU_ASSERT_PTR_EQUAL(int_comparator, sut->comparator);
    CU_ASSERT_PTR_NULL(sut->root);
    CU_ASSERT_EQUAL(0, sut->n);

    SearchTree_Destroy(sut, NULL);
}

/*************************** SearchTree_Insert ********************************/
static void SearchTree_Insert_failed_mem_allocation()
{
    SearchTree* sut = SearchTree_Create(int_comparator);
    int value = 5;
    ListOpResult result;

    FAILED_MALLOC_TEST({ result = SearchTree_Insert(sut, &value); })

    CU_ASSERT_EQUAL(ListOp_FailedMalloc, result);
    SearchTree_Destroy(sut, NULL);
}

static void SearchTree_Insert_creates_root()
{
    SearchTree* sut = SearchTree_Create(int_comparator);
    int value = 5;

    ListOpResult result = SearchTree_Insert(sut, &value);
    CU_ASSERT_EQUAL(ListOp_Success, result);
    CU_ASSERT_PTR_NOT_NULL(sut->root);
    CU_ASSERT_EQUAL(1, sut->n);
    TreeIsValid(sut, 1);

    SearchTree_Destroy(sut, NULL);
}

static void SearchTree_Insert_inits_values()
{
    SearchTree* sut = SearchTree_Create(int_comparator);
    int value = 5;

    SearchTree_Insert(sut, &value);

    SearchTreeNode* node = sut->root;
    CU_ASSERT_PTR_EQUAL(&value, node->item);
    CU_ASSERT_PTR_NULL(node->left);
    CU_ASSERT_PTR_NULL(node->right);
    CU_ASSERT_PTR_NULL(node->parent);
    CU_ASSERT_EQUAL(1, node->size);

    SearchTree_Destroy(sut, NULL);
}

static void SearchTree_Insert_maintains_tree_property()
{
    SUT({ TreePropertyHolds(sut->root); });
}

static void SearchTree_Insert_sets_size()
{
    SUT({ TreeSizeEqualsNodeCount(sut); });
}

static void SearchTree_Insert_creates_valid_tree()
{
    SUT({ TreeIsValid(sut, mock_n); });
}

/*************************** SearchTree_Enumerate *****************************/
static int mock_pos = 0;
static void MockItemHandler(void* item)
{
    CU_ASSERT_EQUAL(mock_ordered[mock_pos], *(int*)item);
    mock_pos++;
}

static void SearchTree_Enumerate_null_paramter()
{
    SUT({
        BINARY_INT_NULL_TEST(
            ListOp_NullParameter, sut, MockItemHandler, SearchTree_Enumerate);
    });
}

static void SearchTree_Enumerate_empty()
{
    EMPTY_SUT({
        ListOpResult result = SearchTree_Enumerate(sut, MockItemHandler);
        CU_ASSERT_EQUAL(ListOp_Success, result);
    });
}

static void SearchTree_Enumerate_standard()
{
    SUT({
        mock_pos = 0;
        ListOpResult result = SearchTree_Enumerate(sut, MockItemHandler);
        CU_ASSERT_EQUAL(ListOp_Success, result);
        CU_ASSERT_EQUAL(mock_n, mock_pos);
    });
}

/*************************** SearchTree_Search ********************************/
static void SearchTree_Search_not_found()
{
    SUT({
        int not_found = 401;
        ErrorReporter_Clear();
        void* result = SearchTree_Search(sut, &not_found);
        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_NotFound, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Search_empty()
{
    EMPTY_SUT({
        int search_for = 5;

        ErrorReporter_Clear();
        void* result = SearchTree_Search(sut, &search_for);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Search_standard()
{
    SUT({
        int* val = mock_vals;

        while (*val != TERMINATOR) {
            void* result = SearchTree_Search(sut, val);
            CU_ASSERT_PTR_NOT_NULL(result);
            CU_ASSERT_EQUAL(*(int*)result, *val);
            val++;
        }
    });
}

/*************************** SearchTree_Min ***********************************/
static void SearchTree_Min_empty()
{
    EMPTY_SUT({
        ErrorReporter_Clear();
        void* result = SearchTree_Min(sut);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Min_standard()
{
    SUT({
        void* result = SearchTree_Min(sut);
        CU_ASSERT_EQUAL(11, *(int*)result);
    });
}

/*************************** SearchTree_Max ***********************************/
static void SearchTree_Max_empty()
{
    EMPTY_SUT({
        ErrorReporter_Clear();
        void* result = SearchTree_Max(sut);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Max_standard()
{
    SUT({
        void* result = SearchTree_Max(sut);
        CU_ASSERT_EQUAL(95, *(int*)result);
    });
}

/*************************** SearchTree_Predecessor ***************************/
static void SearchTree_Predecessor_empty()
{
    EMPTY_SUT({
        int search_for = 5;

        ErrorReporter_Clear();
        void* result = SearchTree_Predecessor(sut, &search_for);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Predecessor_not_found()
{
    SUT({
        int not_found = 401;

        ErrorReporter_Clear();
        void* result = SearchTree_Predecessor(sut, &not_found);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_NotFound, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Predecessor_first_item()
{
    SUT({
        int first = 11;

        ErrorReporter_Clear();
        void* result = SearchTree_Predecessor(sut, &first);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(0, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Predecessor_standard()
{
    SUT({
        for (size_t i = 1; i < mock_n; i++) {
            void* result = SearchTree_Predecessor(sut, &mock_ordered[i]);
            CU_ASSERT_EQUAL(mock_ordered[i - 1], *(int*)result);
        }
    });
}

/*************************** SearchTree_Successor *****************************/
static void SearchTree_Successor_empty()
{
    EMPTY_SUT({
        int search_for = 5;

        ErrorReporter_Clear();
        void* result = SearchTree_Successor(sut, &search_for);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Successor_not_found()
{
    SUT({
        int not_found = 401;

        ErrorReporter_Clear();
        void* result = SearchTree_Successor(sut, &not_found);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_NotFound, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Successor_last_item()
{
    SUT({
        int last = 95;

        ErrorReporter_Clear();
        void* result = SearchTree_Successor(sut, &last);

        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(0, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Successor_standard()
{
    SUT({
        for (size_t i = 0; i < mock_n - 1; i++) {
            void* result = SearchTree_Successor(sut, &mock_ordered[i]);
            CU_ASSERT_EQUAL(mock_ordered[i + 1], *(int*)result);
        }
    });
}

/*************************** SearchTree_Select ********************************/
static void SearchTree_Select_null_paramter()
{
    ErrorReporter_Clear();
    void* result = SearchTree_Select(NULL, 1);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(ListOp_NullParameter, ErrorReporter_LastErrorCode());
}

static void SearchTree_Select_empty()
{
    EMPTY_SUT({
        ErrorReporter_Clear();
        void* result = SearchTree_Select(sut, 0);
        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Select_out_of_bounds()
{
    SUT({
        ErrorReporter_Clear();
        void* result = SearchTree_Select(sut, mock_n + 1);
        CU_ASSERT_PTR_NULL(result);
        CU_ASSERT_EQUAL(ListOp_InvalidIndex, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Select_standard()
{
    SUT({
        for (size_t i = 0; i < mock_n; i++) {
            void* result = SearchTree_Select(sut, i);
            CU_ASSERT_EQUAL(mock_ordered[i], *(int*)result);
        }
    });
}

/*************************** SearchTree_Rank ********************************/
static void SearchTree_Rank_null_paramter()
{
    SUT({
        int search_for = 5;

        ErrorReporter_Clear();
        size_t result = SearchTree_Rank(NULL, NULL);
        CU_ASSERT_EQUAL(RANK_ERROR, result);
        CU_ASSERT_EQUAL(ListOp_NullParameter, ErrorReporter_LastErrorCode());

        ErrorReporter_Clear();
        result = SearchTree_Rank(sut, NULL);
        CU_ASSERT_EQUAL(RANK_ERROR, result);
        CU_ASSERT_EQUAL(ListOp_NullParameter, ErrorReporter_LastErrorCode());

        ErrorReporter_Clear();
        result = SearchTree_Rank(NULL, &search_for);
        CU_ASSERT_EQUAL(RANK_ERROR, result);
        CU_ASSERT_EQUAL(ListOp_NullParameter, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Rank_empty()
{
    EMPTY_SUT({
        int search_for = 5;

        ErrorReporter_Clear();
        size_t result = SearchTree_Rank(sut, &search_for);
        CU_ASSERT_EQUAL(RANK_ERROR, result);
        CU_ASSERT_EQUAL(ListOp_EmptyList, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Rank_not_found()
{
    SUT({
        int not_found = 401;
        ErrorReporter_Clear();
        size_t result = SearchTree_Rank(sut, &not_found);
        CU_ASSERT_EQUAL(RANK_ERROR, result);
        CU_ASSERT_EQUAL(ListOp_NotFound, ErrorReporter_LastErrorCode());
    });
}

static void SearchTree_Rank_standard()
{
    SUT({
        for (size_t i = 0; i < mock_n; i++) {
            size_t result = SearchTree_Rank(sut, &mock_ordered[i]);
            CU_ASSERT_EQUAL(result, i);
        }
    });
}

/*************************** Common *******************************************/
static void SearchTree_null_parameters()
{
    SearchTree* tree = SearchTree_Create(int_comparator);
    int search_for = 5;

    ListOpResult (*binary_int_tests[])(SearchTree*, void*)
        = { SearchTree_Insert, NULL };
    BINARY_INT_NULL_TEST_ENUMERATOR(
        ListOp_NullParameter, tree, &search_for, binary_int_tests);

    void* (*binary_tests[])(const SearchTree*, const void*)
        = { SearchTree_Search, SearchTree_Predecessor, SearchTree_Successor,
              NULL };
    BINARY_NULL_TEST_ENUMERATOR(
        ListOp_NullParameter, tree, &search_for, binary_tests);

    void* (*tests[])(const SearchTree*)
        = { SearchTree_Min, SearchTree_Max, NULL };

    UNARY_NULL_TEST_ENUMERATOR(ListOp_NullParameter, tests);

    SearchTree_Destroy(tree, NULL);
}

static void SearchTree_validate_validator()
{
    SearchTree* sut = ManuallyGenerateValidTree();
    TreeIsValid(sut, 10);
    SearchTree_Destroy(sut, free);
}

int register_search_tree_tests()
{
    CU_TestInfo create_tests[]
        = { CU_TEST_INFO(SearchTree_Create_null_parmeter),
              CU_TEST_INFO(SearchTree_Create_failed_mem_allocation),
              CU_TEST_INFO(SearchTree_Create_init_values), CU_TEST_INFO_NULL };

    CU_TestInfo insert_tests[] = { CU_TEST_INFO(
                                       SearchTree_Insert_failed_mem_allocation),
        CU_TEST_INFO(SearchTree_Insert_creates_root),
        CU_TEST_INFO(SearchTree_Insert_inits_values),
        CU_TEST_INFO(SearchTree_Insert_maintains_tree_property),
        CU_TEST_INFO(SearchTree_Insert_sets_size),
        CU_TEST_INFO(SearchTree_Insert_creates_valid_tree), CU_TEST_INFO_NULL };

    CU_TestInfo enumerate_tests[]
        = { CU_TEST_INFO(SearchTree_Enumerate_null_paramter),
              CU_TEST_INFO(SearchTree_Enumerate_empty),
              CU_TEST_INFO(SearchTree_Enumerate_standard), CU_TEST_INFO_NULL };

    CU_TestInfo search_tests[] = { CU_TEST_INFO(SearchTree_Search_not_found),
        CU_TEST_INFO(SearchTree_Search_standard),
        CU_TEST_INFO(SearchTree_Search_empty), CU_TEST_INFO_NULL };

    CU_TestInfo min_max_tests[] = { CU_TEST_INFO(SearchTree_Min_empty),
        CU_TEST_INFO(SearchTree_Min_standard),
        CU_TEST_INFO(SearchTree_Max_empty),
        CU_TEST_INFO(SearchTree_Max_standard), CU_TEST_INFO_NULL };

    CU_TestInfo predecessor_tests[] = { CU_TEST_INFO(
                                            SearchTree_Predecessor_empty),
        CU_TEST_INFO(SearchTree_Predecessor_not_found),
        CU_TEST_INFO(SearchTree_Predecessor_first_item),
        CU_TEST_INFO(SearchTree_Predecessor_standard), CU_TEST_INFO_NULL };

    CU_TestInfo successor_tests[] = { CU_TEST_INFO(SearchTree_Successor_empty),
        CU_TEST_INFO(SearchTree_Successor_not_found),
        CU_TEST_INFO(SearchTree_Successor_last_item),
        CU_TEST_INFO(SearchTree_Successor_standard), CU_TEST_INFO_NULL };

    CU_TestInfo select_tests[]
        = { CU_TEST_INFO(SearchTree_Select_null_paramter),
              CU_TEST_INFO(SearchTree_Select_empty),
              CU_TEST_INFO(SearchTree_Select_out_of_bounds),
              CU_TEST_INFO(SearchTree_Select_standard), CU_TEST_INFO_NULL };

    CU_TestInfo rank_tests[] = { CU_TEST_INFO(SearchTree_Rank_null_paramter),
        CU_TEST_INFO(SearchTree_Rank_empty),
        CU_TEST_INFO(SearchTree_Rank_not_found),
        CU_TEST_INFO(SearchTree_Rank_standard), CU_TEST_INFO_NULL };

    CU_TestInfo common_tests[] = { CU_TEST_INFO(SearchTree_null_parameters),
        CU_TEST_INFO(SearchTree_validate_validator), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "SearchTree_Create",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = create_tests },
        { .pName = "SearchTree_Insert",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = insert_tests },
        { .pName = "SearchTree_Enumerate",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = enumerate_tests },
        { .pName = "SearchTree_Search",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = search_tests },
        { .pName = "SearchTree_Min and SearchTree_Max",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = min_max_tests },
        { .pName = "SearchTree_Predecessor",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = predecessor_tests },
        { .pName = "SearchTree_Successor",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = successor_tests },
        { .pName = "SearchTree_Select",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = select_tests },
        { .pName = "SearchTree_Rank",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = rank_tests },
        { .pName = "SearchTree Common",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = common_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}

/* ListOpResult SearchTree_Insert(SearchTree*, void*); */
/* ListOpResult SearchTree_Delete(SearchTree*, void*); */
/* void SearchTree_Destroy(SearchTree*, freer); */
