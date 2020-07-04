/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "binary_tree.h"

#include <limits.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "common_math.h"
#include "error_reporter.h"
#include "malloc_test_wrapper.h"
#include "test_helpers.h"

#define SUT(vals, code_block)           \
  {                                     \
    BinaryTree* sut = create_sut(vals); \
    code_block;                         \
    BinaryTree_Destroy(sut, NULL);      \
  }

#define RED_BLACK_SUT(vals, code_block)           \
  {                                               \
    BinaryTree* sut = create_red_black_sut(vals); \
    code_block;                                   \
    BinaryTree_Destroy(sut, NULL);                \
  }

static const int TERMINATOR = INT_MIN;
static const size_t mock_n = 13;

static int empty_vals[] = {TERMINATOR};
static int mock_vals[] = {50, 25, 33, 11, 30, 40, 75,
                          61, 89, 52, 82, 95, 55, TERMINATOR};
static const int mock_ordered[] = {11, 25, 30, 33, 40, 50, 52,
                                   55, 61, 75, 82, 89, 95, TERMINATOR};

static int node_value(BinaryTreeNode* node) {
  if (node == &kNullNode) return -1;

  if (node == NULL) return -2;

  return *(int*)node->payload;
}

static BinaryTree* create_sut(int* vals) {
  BinaryTree* tree = NULL;
  ResultCode result_code = BinaryTree_Create(PIntComparator, &tree);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  while (*vals != TERMINATOR) {
    BinaryTree_Insert(tree, vals);
    vals++;
  }

  return tree;
}

static BinaryTree* create_red_black_sut(int* vals) {
  BinaryTree* tree = NULL;
  ResultCode result_code = BinaryTree_Create(PIntComparator, &tree);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  while (*vals != TERMINATOR) {
    RedBlackTree_Insert(tree, vals);
    vals++;
  }

  return tree;
}

static size_t node_count(BinaryTreeNode* node) {
  if (node == &kNullNode) return 0;

  size_t nc = node_count(node->left) + node_count(node->right) + 1;
  if (nc != node->size) {
    int node_id = *(int*)node->payload;
    printf("invalid size: id=%d expected=%zu, actual=%zu\n", node_id, nc,
           node->size);
    CU_FAIL();
  }
  return nc;
}

static void size_equals_node_count(BinaryTree* tree) {
  CU_ASSERT_EQUAL(tree->n, node_count(tree->root));
}

static void node_is_valid(BinaryTreeNode* node) {
  if (node == &kNullNode) return;

  int node_id = *(int*)node->payload;

  if (node->left != &kNullNode) {
    int result = PIntComparator(node->payload, node->left->payload);
    if (result < 0) {
      printf("ASSERT FAILURE on node %d - left item is greater than current",
             node_id);
      CU_FAIL();
    }
  }

  if (node->right != &kNullNode) {
    int result = PIntComparator(node->payload, node->right->payload);
    if (result > 0) {
      printf("ASSERT FAILURE on node %d - right item is less than current",
             node_id);
      CU_FAIL();
    }
  }
}

static void tree_property_holds(BinaryTreeNode* node) {
  if (node == &kNullNode) return;

  tree_property_holds(node->left);
  node_is_valid(node);
  tree_property_holds(node->right);
}

static void tree_is_valid(BinaryTree* tree, size_t n) {
  if (tree == NULL) {
    CU_FAIL("tree is null");
    return;
  }

  if (n == 0) {
    CU_ASSERT_PTR_EQUAL(&kNullNode, tree->root);
  } else {
    CU_ASSERT_EQUAL(tree->n, tree->root->size);
  }

  CU_ASSERT_EQUAL(n, tree->n);
  size_equals_node_count(tree);
  tree_property_holds(tree->root);
}

static BinaryTree* generate_valid_tree() {
  const size_t n = 10;
  BinaryTree* sut = NULL;
  ResultCode result_code = BinaryTree_Create(PIntComparator, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  BinaryTreeNode* nodes[n];
  for (size_t i = 0; i < n; i++) {
    size_t* val = malloc(sizeof(size_t));
    *val = i;

    nodes[i] = calloc(sizeof(BinaryTreeNode), 1);
    nodes[i]->payload = val;
  }

  nodes[0]->parent = nodes[1];
  nodes[0]->left = &kNullNode;
  nodes[0]->right = &kNullNode;
  nodes[0]->size = 1;

  nodes[1]->parent = nodes[2];
  nodes[1]->left = nodes[0];
  nodes[1]->right = nodes[3];
  nodes[1]->size = 4;

  nodes[2]->parent = &kNullNode;
  nodes[2]->left = nodes[1];
  nodes[2]->right = nodes[7];
  nodes[2]->size = 10;

  nodes[3]->parent = nodes[1];
  nodes[3]->left = &kNullNode;
  nodes[3]->right = nodes[4];
  nodes[3]->size = 2;

  nodes[4]->parent = nodes[3];
  nodes[4]->left = &kNullNode;
  nodes[4]->right = &kNullNode;
  nodes[4]->size = 1;

  nodes[5]->parent = nodes[6];
  nodes[5]->left = &kNullNode;
  nodes[5]->right = &kNullNode;
  nodes[5]->size = 1;

  nodes[6]->parent = nodes[7];
  nodes[6]->left = nodes[5];
  nodes[6]->right = &kNullNode;
  nodes[6]->size = 2;

  nodes[7]->parent = nodes[2];
  nodes[7]->left = nodes[6];
  nodes[7]->right = nodes[8];
  nodes[7]->size = 5;

  nodes[8]->parent = nodes[7];
  nodes[8]->left = &kNullNode;
  nodes[8]->right = nodes[9];
  nodes[8]->size = 2;

  nodes[9]->parent = nodes[8];
  nodes[9]->left = &kNullNode;
  nodes[9]->right = &kNullNode;
  nodes[9]->size = 1;

  sut->root = nodes[2];
  sut->n = n;
  return sut;
}

/*************************** BinaryTree_Create ********************************/
static void BinaryTree_Create_null_parmeter() {
  BinaryTree* sut = NULL;
  ResultCode result_code = BinaryTree_Create(NULL, &sut);
  CU_ASSERT_EQUAL(result_code, kNullParameter);

  sut = malloc(sizeof(BinaryTree));
  result_code = BinaryTree_Create(PIntComparator, &sut);
  CU_ASSERT_EQUAL(result_code, kOutputPointerIsNotNull);

  free(sut);
}

static void BinaryTree_Create_failed_mem_allocation() {
#if !defined(NDEBUG)
  BinaryTree* sut = NULL;
  ResultCode result_code;

  FAILED_MALLOC_TEST({ result_code = BinaryTree_Create(PIntComparator, &sut); })

  CU_ASSERT_PTR_NULL(sut);
  CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
#endif
}

static void BinaryTree_Create_init_values() {
  BinaryTree* sut = NULL;
  ResultCode result_code = BinaryTree_Create(PIntComparator, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  CU_ASSERT_PTR_EQUAL(PIntComparator, sut->comparator);
  CU_ASSERT_PTR_EQUAL(&kNullNode, sut->root);
  CU_ASSERT_EQUAL(0, sut->n);

  BinaryTree_Destroy(sut, NULL);
}

/*************************** BinaryTree_Insert ********************************/
static void BinaryTree_Insert_failed_mem_allocation() {
  BinaryTree* sut = NULL;
  ResultCode result_code = BinaryTree_Create(PIntComparator, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  int value = 5;

  FAILED_MALLOC_TEST({ result_code = BinaryTree_Insert(sut, &value); })

  CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
  BinaryTree_Destroy(sut, NULL);
}

static void BinaryTree_Insert_creates_root() {
  BinaryTree* sut = NULL;
  ResultCode result_code = BinaryTree_Create(PIntComparator, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  int value = 5;

  ResultCode result = BinaryTree_Insert(sut, &value);
  CU_ASSERT_EQUAL(result, kSuccess);
  CU_ASSERT_PTR_NOT_EQUAL(&kNullNode, sut->root);
  CU_ASSERT_EQUAL(1, sut->n);
  tree_is_valid(sut, 1);

  BinaryTree_Destroy(sut, NULL);
}

static void BinaryTree_Insert_inits_values() {
  BinaryTree* sut = NULL;
  ResultCode result_code = BinaryTree_Create(PIntComparator, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  int value = 5;

  result_code = BinaryTree_Insert(sut, &value);
  CU_ASSERT_EQUAL(result_code, kSuccess);

  BinaryTreeNode* node = sut->root;
  CU_ASSERT_PTR_EQUAL(&value, node->payload);
  CU_ASSERT_PTR_EQUAL(&kNullNode, node->left);
  CU_ASSERT_PTR_EQUAL(&kNullNode, node->right);
  CU_ASSERT_PTR_EQUAL(&kNullNode, node->parent);
  CU_ASSERT_EQUAL(1, node->size);

  BinaryTree_Destroy(sut, NULL);
}

static void BinaryTree_Insert_maintains_tree_property() {
  SUT(mock_vals, { tree_property_holds(sut->root); });
}

static void BinaryTree_Insert_sets_size() {
  SUT(mock_vals, { size_equals_node_count(sut); });
}

static void BinaryTree_Insert_creates_valid_tree() {
  SUT(mock_vals, { tree_is_valid(sut, mock_n); });
}

/*************************** BinaryTree_Delete ********************************/
static void BinaryTree_Delete_null_paramter() {
  SUT(mock_vals, {
    int search_for = 5;
    void* result = NULL;

    ResultCode result_code = BinaryTree_Delete(NULL, &search_for, &result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code = BinaryTree_Delete(sut, NULL, &result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result = malloc(sizeof(int));
    result_code = BinaryTree_Delete(sut, &search_for, &result);
    CU_ASSERT_EQUAL(result_code, kOutputPointerIsNotNull);
    free(result);
  });
}

static void BinaryTree_Delete_empty() {
  SUT(empty_vals, {
    int search_for = 5;
    void* result = NULL;

    ResultCode result_code = BinaryTree_Delete(sut, &search_for, &result);
    CU_ASSERT_EQUAL(result_code, kEmpty);
  });
}

static void BinaryTree_Delete_not_found() {
  SUT(mock_vals, {
    int not_found = 401;
    void* result = NULL;
    ResultCode result_code = BinaryTree_Delete(sut, &not_found, &result);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kNotFound);
    tree_is_valid(sut, mock_n);
  });
}

static void BinaryTree_DeleteTest(int doomed) {
  SUT(mock_vals, {
    void* result = NULL;
    ResultCode result_code = BinaryTree_Delete(sut, &doomed, &result);

    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(doomed, *(int*)result);
    tree_is_valid(sut, mock_n - 1);

    void* found = NULL;
    result_code = BinaryTree_Search(sut, &doomed, &found);
    CU_ASSERT_PTR_NULL(found);
    CU_ASSERT_EQUAL(result_code, kNotFound);
  });
}

static void BinaryTree_Delete_leaf() { BinaryTree_DeleteTest(11); }

static void BinaryTree_Delete_degree_1_node() { BinaryTree_DeleteTest(52); }

static void BinaryTree_Delete_degree_2_node() { BinaryTree_DeleteTest(75); }

static void BinaryTree_Delete_root() { BinaryTree_DeleteTest(50); }

static void BinaryTree_Delete_last_item() {
  SUT(mock_vals, {
    for (size_t i = 0; i < mock_n; i++) {
      int doomed = mock_ordered[i];
      void* result = NULL;
      ResultCode result_code = BinaryTree_Delete(sut, &doomed, &result);
      CU_ASSERT_EQUAL(result_code, kSuccess);
      CU_ASSERT_EQUAL(doomed, *(int*)result);
      tree_is_valid(sut, mock_n - i - 1);
    }

    CU_ASSERT_EQUAL(0, sut->n);
    CU_ASSERT_PTR_EQUAL(&kNullNode, sut->root);
  });
}

static void BinaryTree_Delete_standard() {
  for (size_t i = 0; i < mock_n; i++) {
    SUT(mock_vals, {
      int doomed = mock_ordered[i];
      void* result = NULL;
      ResultCode result_code = BinaryTree_Delete(sut, &doomed, &result);
      CU_ASSERT_EQUAL(result_code, kSuccess);
      CU_ASSERT_EQUAL(doomed, *(int*)result);
      tree_is_valid(sut, mock_n - 1);

      void* found = NULL;
      result_code = BinaryTree_Search(sut, &doomed, &found);
      CU_ASSERT_PTR_NULL(found);
      CU_ASSERT_EQUAL(result_code, kNotFound);
    });
  }
}

/*************************** BinaryTree_Enumerate *****************************/
static size_t mock_pos = 0;
static void MockItemHandler(void* item) {
  CU_ASSERT_EQUAL(mock_ordered[mock_pos], *(int*)item);
  mock_pos++;
}

static void BinaryTree_Enumerate_null_paramter() {
  SUT(mock_vals, {
    ResultCode result_code = BinaryTree_Enumerate(NULL, MockItemHandler);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code = BinaryTree_Enumerate(sut, NULL);
    CU_ASSERT_EQUAL(result_code, kNullParameter);
  });
}

static void BinaryTree_Enumerate_empty() {
  SUT(empty_vals, {
    ResultCode result_code = BinaryTree_Enumerate(sut, MockItemHandler);
    CU_ASSERT_EQUAL(result_code, kSuccess);
  });
}

static void BinaryTree_Enumerate_standard() {
  SUT(mock_vals, {
    mock_pos = 0;
    ResultCode result_code = BinaryTree_Enumerate(sut, MockItemHandler);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    CU_ASSERT_EQUAL(mock_n, mock_pos);
  });
}

/*************************** BinaryTree_Search ********************************/
static void BinaryTree_Search_not_found() {
  SUT(mock_vals, {
    int not_found = 401;
    void* result = NULL;
    ResultCode result_code = BinaryTree_Search(sut, &not_found, &result);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kNotFound);
  });
}

static void BinaryTree_Search_empty() {
  SUT(empty_vals, {
    int search_for = 5;

    void* result = NULL;
    ResultCode result_code = BinaryTree_Search(sut, &search_for, &result);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kEmpty);
  });
}

static void BinaryTree_Search_standard() {
  SUT(mock_vals, {
    int* val = mock_vals;

    while (*val != TERMINATOR) {
      void* result = NULL;
      ResultCode result_code = BinaryTree_Search(sut, val, &result);

      CU_ASSERT_PTR_NOT_NULL(result);
      CU_ASSERT_EQUAL(result_code, kSuccess);

      if (result == NULL) {
        CU_FAIL("result is NULL");
      } else {
        CU_ASSERT_EQUAL(*(int*)result, *val);
      }

      val++;
    }
  });
}

/*************************** BinaryTree_Min ***********************************/
static void BinaryTree_Min_empty() {
  SUT(empty_vals, {
    void* result = NULL;
    ResultCode result_code = BinaryTree_Min(sut, &result);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kEmpty);
  });
}

static void BinaryTree_Min_standard() {
  SUT(mock_vals, {
    void* result = NULL;
    ResultCode result_code = BinaryTree_Min(sut, &result);
    CU_ASSERT_EQUAL(11, *(int*)result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
  });
}

/*************************** BinaryTree_Max ***********************************/
static void BinaryTree_Max_empty() {
  SUT(empty_vals, {
    void* result = NULL;
    ResultCode result_code = BinaryTree_Max(sut, &result);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kEmpty);
  });
}

static void BinaryTree_Max_standard() {
  SUT(mock_vals, {
    void* result = NULL;
    ResultCode result_code = BinaryTree_Max(sut, &result);
    CU_ASSERT_EQUAL(95, *(int*)result);
    CU_ASSERT_EQUAL(result_code, kSuccess);
  });
}

/*************************** BinaryTree_Predecessor ***************************/
static void BinaryTree_Predecessor_empty() {
  SUT(empty_vals, {
    int search_for = 5;

    void* result = NULL;
    ResultCode result_code = BinaryTree_Predecessor(sut, &search_for, &result);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kEmpty);
  });
}

static void BinaryTree_Predecessor_not_found() {
  SUT(mock_vals, {
    int not_found = 401;

    void* result = NULL;
    ResultCode result_code = BinaryTree_Predecessor(sut, &not_found, &result);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kNotFound);
  });
}

static void BinaryTree_Predecessor_first_item() {
  SUT(mock_vals, {
    int first = 11;

    void* result = NULL;
    ResultCode result_code = BinaryTree_Predecessor(sut, &first, &result);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);
  });
}

static void BinaryTree_Predecessor_standard() {
  SUT(mock_vals, {
    for (size_t i = 1; i < mock_n; i++) {
      void* result = NULL;
      ResultCode result_code =
          BinaryTree_Predecessor(sut, &mock_ordered[i], &result);
      CU_ASSERT_EQUAL(result_code, kSuccess);
      CU_ASSERT_EQUAL(mock_ordered[i - 1], *(int*)result);
    }
  });
}

/*************************** BinaryTree_Successor *****************************/
static void BinaryTree_Successor_empty() {
  SUT(empty_vals, {
    int search_for = 5;

    void* result = NULL;
    ResultCode result_code = BinaryTree_Successor(sut, &search_for, &result);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kEmpty);
  });
}

static void BinaryTree_Successor_not_found() {
  SUT(mock_vals, {
    int not_found = 401;

    void* result = NULL;
    ResultCode result_code = BinaryTree_Successor(sut, &not_found, &result);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kNotFound);
  });
}

static void BinaryTree_Successor_last_item() {
  SUT(mock_vals, {
    int last = 95;

    void* result = NULL;
    ResultCode result_code = BinaryTree_Successor(sut, &last, &result);
    printf("%d\n", result_code);

    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kArgumentOutOfRange);
  });
}

static void BinaryTree_Successor_standard() {
  SUT(mock_vals, {
    for (size_t i = 0; i < mock_n - 1; i++) {
      void* result = NULL;
      ResultCode result_code =
          BinaryTree_Successor(sut, &mock_ordered[i], &result);
      CU_ASSERT_EQUAL(result_code, kSuccess);
      CU_ASSERT_EQUAL(mock_ordered[i + 1], *(int*)result);
    }
  });
}

/*************************** BinaryTree_Select ********************************/
static void BinaryTree_Select_null_paramter() {
  void* result = NULL;
  ResultCode result_code = BinaryTree_Select(NULL, 1, &result);
  CU_ASSERT_PTR_NULL(result);
  CU_ASSERT_EQUAL(result_code, kNullParameter);
}

static void BinaryTree_Select_empty() {
  SUT(empty_vals, {
    void* result = NULL;
    ResultCode result_code = BinaryTree_Select(sut, 0, &result);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kEmpty);
  });
}

static void BinaryTree_Select_out_of_bounds() {
  SUT(mock_vals, {
    void* result = NULL;
    ResultCode result_code = BinaryTree_Select(sut, mock_n + 1, &result);
    CU_ASSERT_PTR_NULL(result);
    CU_ASSERT_EQUAL(result_code, kInvalidIndex);
  });
}

static void BinaryTree_Select_standard() {
  SUT(mock_vals, {
    for (size_t i = 0; i < mock_n; i++) {
      void* result = NULL;
      ResultCode result_code = BinaryTree_Select(sut, i, &result);
      CU_ASSERT_EQUAL(mock_ordered[i], *(int*)result);
      CU_ASSERT_EQUAL(result_code, kSuccess);
    }
  });
}

/*************************** BinaryTree_Rank **********************************/
static void BinaryTree_Rank_null_paramter() {
  SUT(mock_vals, {
    int search_for = 5;

    size_t result = 0;

    ResultCode result_code = BinaryTree_Rank(NULL, NULL, &result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code = BinaryTree_Rank(sut, NULL, &result);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code = BinaryTree_Rank(sut, &search_for, NULL);
    CU_ASSERT_EQUAL(result_code, kNullParameter);
  });
}

static void BinaryTree_Rank_empty() {
  SUT(empty_vals, {
    int search_for = 5;

    size_t result = 0;
    ResultCode result_code = BinaryTree_Rank(sut, &search_for, &result);
    CU_ASSERT_EQUAL(result_code, kEmpty);
  });
}

static void BinaryTree_Rank_not_found() {
  SUT(mock_vals, {
    int not_found = 401;
    size_t result = 0;
    ResultCode result_code = BinaryTree_Rank(sut, &not_found, &result);
    CU_ASSERT_EQUAL(result_code, kNotFound);
  });
}

static void BinaryTree_Rank_standard() {
  SUT(mock_vals, {
    for (size_t i = 0; i < mock_n; i++) {
      size_t result = 0;
      ResultCode result_code = BinaryTree_Rank(sut, &mock_ordered[i], &result);
      CU_ASSERT_EQUAL(result, i);
      CU_ASSERT_EQUAL(result_code, kSuccess);
    }
  });
}

/*************************** BinaryTree_Destory *******************************/
static void BinaryTree_Destroy_null() {
  // if this does not blow up, it passes
  BinaryTree_Destroy(NULL, NULL);
}

size_t free_tracker = 0;
static void TestFreer(void* val) {
  (void)val;
  free_tracker++;
}

static void BinaryTree_Destroy_runs_freer() {
  free_tracker = 0;
  BinaryTree* sut = create_sut(mock_vals);
  BinaryTree_Destroy(sut, TestFreer);
  CU_ASSERT_EQUAL(mock_n, free_tracker);
}

/*************************** Common *******************************************/
static void BinaryTree_validate_validator() {
  BinaryTree* sut = generate_valid_tree();
  tree_is_valid(sut, 10);
  BinaryTree_Destroy(sut, free);
}

/*************************** BinaryTree_RotateLeft ****************************/
static void TreeHasValues(BinaryTreeNode* sut, BinaryTreeNode* expected) {
  if (expected == NULL || sut == NULL) return;

  int actual = node_value(sut);
  int exp = node_value(expected);

  if (exp != actual) {
    printf("expected=%d, node actual=%d\n", exp, actual);
    CU_FAIL("Unexpected value");
  }

  if (expected == &kNullNode || sut == &kNullNode) return;

  TreeHasValues(sut->left, expected->left);
  TreeHasValues(sut->right, expected->right);
}

static void test_rotate(int* vals, int* expected_vals, int pivot, int n,
                        ResultCode (*rotate)(BinaryTree*, const void*)) {
  SUT(vals, {
    ResultCode result_code = rotate(sut, &pivot);
    CU_ASSERT_EQUAL(result_code, kSuccess);
    tree_is_valid(sut, n);

    BinaryTree* expected = create_sut(expected_vals);
    TreeHasValues(sut->root, expected->root);
    BinaryTree_Destroy(expected, NULL);
  });
}

static void BinaryTree_RotateLeft_null_parameter() {
  int search_for = 5;
  SUT(mock_vals, {
    ResultCode result_code = BinaryTree_RotateLeft(NULL, &search_for);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code = BinaryTree_RotateLeft(sut, NULL);
    CU_ASSERT_EQUAL(result_code, kNullParameter);
  });
}

static void BinaryTree_RotateLeft_not_found() {
  int not_found = 401;

  SUT(mock_vals, {
    ResultCode result_code = BinaryTree_RotateLeft(sut, &not_found);
    CU_ASSERT_EQUAL(result_code, kNotFound);
  });
}

static void BinaryTree_RotateLeft_simple() {
  int vals[] = {30, 50, 75, TERMINATOR};
  int expected_vals[] = {50, 30, 75, TERMINATOR};
  test_rotate(vals, expected_vals, 30, 3, BinaryTree_RotateLeft);
}

static void BinaryTree_RotateLeft_happy_path() {
  int vals[] = {7, 4, 11, 3, 6, 9, 18, 2, 14, 19, 12, 17, 22, 20, TERMINATOR};
  int expected_vals[] = {7, 4,  18, 3,  6,  11, 19,        2,
                         9, 14, 22, 12, 17, 20, TERMINATOR};
  test_rotate(vals, expected_vals, 11, 14, BinaryTree_RotateLeft);

  int vals2[] = {50, 25, 75, 61, 89, TERMINATOR};
  int expected_vals2[] = {75, 50, 89, 25, 61, TERMINATOR};
  test_rotate(vals2, expected_vals2, 50, 5, BinaryTree_RotateLeft);
}

static void BinaryTree_RotateRight_null_parameter() {
  int search_for = 5;
  SUT(mock_vals, {
    ResultCode result_code = BinaryTree_RotateRight(NULL, &search_for);
    CU_ASSERT_EQUAL(result_code, kNullParameter);

    result_code = BinaryTree_RotateRight(sut, NULL);
    CU_ASSERT_EQUAL(result_code, kNullParameter);
  });
}

static void BinaryTree_RotateRight_not_found() {
  int not_found = 401;

  SUT(mock_vals, {
    ResultCode result_code = BinaryTree_RotateRight(sut, &not_found);
    CU_ASSERT_EQUAL(result_code, kNotFound);
  });
}

static void BinaryTree_RotateRight_simple() {
  int vals[] = {30, 25, 20, TERMINATOR};
  int expected_vals[] = {25, 20, 30, TERMINATOR};
  test_rotate(vals, expected_vals, 30, 3, BinaryTree_RotateRight);
}

static void BinaryTree_RotateRight_happy_path() {
  int vals[] = {7, 4, 18, 3, 6, 11, 19, 2, 9, 14, 22, 12, 17, 20, TERMINATOR};
  int expected_vals[] = {7,  4,  11, 3,  6,  9,  18,        2,
                         14, 19, 12, 17, 22, 20, TERMINATOR};
  test_rotate(vals, expected_vals, 18, 14, BinaryTree_RotateRight);

  int vals2[] = {50, 25, 75, 61, 89, 52, 65, TERMINATOR};
  int expected_vals2[] = {50, 25, 61, 52, 75, 65, 89, TERMINATOR};
  test_rotate(vals2, expected_vals2, 75, 7, BinaryTree_RotateRight);
}

/*************************** RedBlackTree_Insert ******************************/
// Every node is designated as either red or black
static void red_black_invaiant_1(BinaryTreeNode* node) {
  if (node == NULL) return;

  red_black_invaiant_1(node->left);
  CU_ASSERT_TRUE(node->color == kRed || node->color == kBlack);
  red_black_invaiant_1(node->right);
}

// Root node is black
static void red_black_invaiant_2(BinaryTree* tree) {
  CU_ASSERT_TRUE(tree->root->color == kBlack);
}

// No consecutive red nodes
static void red_black_invaiant_3(BinaryTreeNode* node) {
  if (node == &kNullNode) return;

  red_black_invaiant_3(node->left);
  if (node->color == kRed) {
    CU_ASSERT_TRUE(node->parent->color == kBlack);
    CU_ASSERT_TRUE(node->left->color == kBlack);
    CU_ASSERT_TRUE(node->right->color == kBlack);
  }
  red_black_invaiant_3(node->right);
}

static void red_black_tree_is_valid(BinaryTree* tree, size_t n) {
  tree_is_valid(tree, n);
  red_black_invaiant_1(tree->root);
  red_black_invaiant_2(tree);
  red_black_invaiant_3(tree->root);
}

static void RedBlackTree_Insert_failed_mem_allocation() {
  BinaryTree* sut = NULL;
  ResultCode result_code = BinaryTree_Create(PIntComparator, &sut);
  CU_ASSERT_EQUAL(result_code, kSuccess);
  int value = 5;

  FAILED_MALLOC_TEST({ result_code = RedBlackTree_Insert(sut, &value); })

  CU_ASSERT_EQUAL(result_code, kFailedMemoryAllocation);
  BinaryTree_Destroy(sut, NULL);
}

static void RedBlackTree_Insert_creates_root() {
  int vals[] = {5, TERMINATOR};
  RED_BLACK_SUT(vals, {
    CU_ASSERT_PTR_NOT_EQUAL(&kNullNode, sut->root);
    red_black_tree_is_valid(sut, 1);
  });
}

static void RedBlackTree_Insert_sets_first_child_red() {
  int vals[] = {5, 10, 2, TERMINATOR};
  RED_BLACK_SUT(vals, {
    CU_ASSERT_EQUAL(kRed, sut->root->right->color);
    CU_ASSERT_EQUAL(kRed, sut->root->left->color);
    red_black_tree_is_valid(sut, 3);
  });
}

static void RedBlackTree_Insert_recolor() {
  int vals[] = {10, 5, 15, 12, TERMINATOR};
  RED_BLACK_SUT(vals, {
    CU_ASSERT_EQUAL(10, *(int*)sut->root->payload);
    red_black_tree_is_valid(sut, 4);
  });
}

static void RedBlackTree_Insert_rotations() {
  int vals[] = {11, 2, 1, 7, 8, 5, 14, 15, 4, 16, TERMINATOR};
  RED_BLACK_SUT(vals, { red_black_tree_is_valid(sut, 10); });

  int vals2[] = {50, 25, 100, 10, 30, 75, 150, 60, 80, 200, 55, TERMINATOR};
  RED_BLACK_SUT(vals2, { red_black_tree_is_valid(sut, 11); });
}

int RegisterBinaryTreeTests() {
  CU_TestInfo create_tests[] = {
      CU_TEST_INFO(BinaryTree_Create_null_parmeter),
      CU_TEST_INFO(BinaryTree_Create_failed_mem_allocation),
      CU_TEST_INFO(BinaryTree_Create_init_values), CU_TEST_INFO_NULL};

  CU_TestInfo insert_tests[] = {
      CU_TEST_INFO(BinaryTree_Insert_failed_mem_allocation),
      CU_TEST_INFO(BinaryTree_Insert_creates_root),
      CU_TEST_INFO(BinaryTree_Insert_inits_values),
      CU_TEST_INFO(BinaryTree_Insert_maintains_tree_property),
      CU_TEST_INFO(BinaryTree_Insert_sets_size),
      CU_TEST_INFO(BinaryTree_Insert_creates_valid_tree),
      CU_TEST_INFO_NULL};

  CU_TestInfo delete_tests[] = {CU_TEST_INFO(BinaryTree_Delete_null_paramter),
                                CU_TEST_INFO(BinaryTree_Delete_empty),
                                CU_TEST_INFO(BinaryTree_Delete_not_found),
                                CU_TEST_INFO(BinaryTree_Delete_leaf),
                                CU_TEST_INFO(BinaryTree_Delete_degree_1_node),
                                CU_TEST_INFO(BinaryTree_Delete_degree_2_node),
                                CU_TEST_INFO(BinaryTree_Delete_root),
                                CU_TEST_INFO(BinaryTree_Delete_standard),
                                CU_TEST_INFO(BinaryTree_Delete_last_item),
                                CU_TEST_INFO_NULL};

  CU_TestInfo enumerate_tests[] = {
      CU_TEST_INFO(BinaryTree_Enumerate_null_paramter),
      CU_TEST_INFO(BinaryTree_Enumerate_empty),
      CU_TEST_INFO(BinaryTree_Enumerate_standard), CU_TEST_INFO_NULL};

  CU_TestInfo search_tests[] = {CU_TEST_INFO(BinaryTree_Search_not_found),
                                CU_TEST_INFO(BinaryTree_Search_standard),
                                CU_TEST_INFO(BinaryTree_Search_empty),
                                CU_TEST_INFO_NULL};

  CU_TestInfo min_max_tests[] = {
      CU_TEST_INFO(BinaryTree_Min_empty), CU_TEST_INFO(BinaryTree_Min_standard),
      CU_TEST_INFO(BinaryTree_Max_empty), CU_TEST_INFO(BinaryTree_Max_standard),
      CU_TEST_INFO_NULL};

  CU_TestInfo predecessor_tests[] = {
      CU_TEST_INFO(BinaryTree_Predecessor_empty),
      CU_TEST_INFO(BinaryTree_Predecessor_not_found),
      CU_TEST_INFO(BinaryTree_Predecessor_first_item),
      CU_TEST_INFO(BinaryTree_Predecessor_standard), CU_TEST_INFO_NULL};

  CU_TestInfo Successor_tests[] = {CU_TEST_INFO(BinaryTree_Successor_empty),
                                   CU_TEST_INFO(BinaryTree_Successor_not_found),
                                   CU_TEST_INFO(BinaryTree_Successor_last_item),
                                   CU_TEST_INFO(BinaryTree_Successor_standard),
                                   CU_TEST_INFO_NULL};

  CU_TestInfo select_tests[] = {CU_TEST_INFO(BinaryTree_Select_null_paramter),
                                CU_TEST_INFO(BinaryTree_Select_empty),
                                CU_TEST_INFO(BinaryTree_Select_out_of_bounds),
                                CU_TEST_INFO(BinaryTree_Select_standard),
                                CU_TEST_INFO_NULL};

  CU_TestInfo rank_tests[] = {CU_TEST_INFO(BinaryTree_Rank_null_paramter),
                              CU_TEST_INFO(BinaryTree_Rank_empty),
                              CU_TEST_INFO(BinaryTree_Rank_not_found),
                              CU_TEST_INFO(BinaryTree_Rank_standard),
                              CU_TEST_INFO_NULL};

  CU_TestInfo destroy_tests[] = {CU_TEST_INFO(BinaryTree_Destroy_null),
                                 CU_TEST_INFO(BinaryTree_Destroy_runs_freer),
                                 CU_TEST_INFO_NULL};

  CU_TestInfo common_tests[] = {CU_TEST_INFO(BinaryTree_validate_validator),
                                CU_TEST_INFO_NULL};

  CU_TestInfo rotate_tests[] = {
      CU_TEST_INFO(BinaryTree_RotateLeft_null_parameter),
      CU_TEST_INFO(BinaryTree_RotateLeft_not_found),
      CU_TEST_INFO(BinaryTree_RotateLeft_simple),
      CU_TEST_INFO(BinaryTree_RotateLeft_happy_path),
      CU_TEST_INFO(BinaryTree_RotateRight_null_parameter),
      CU_TEST_INFO(BinaryTree_RotateRight_not_found),
      CU_TEST_INFO(BinaryTree_RotateRight_simple),
      CU_TEST_INFO(BinaryTree_RotateRight_happy_path),
      CU_TEST_INFO_NULL};

  CU_TestInfo red_black_insert_tests[] = {
      CU_TEST_INFO(RedBlackTree_Insert_failed_mem_allocation),
      CU_TEST_INFO(RedBlackTree_Insert_creates_root),
      CU_TEST_INFO(RedBlackTree_Insert_sets_first_child_red),
      CU_TEST_INFO(RedBlackTree_Insert_recolor),
      CU_TEST_INFO(RedBlackTree_Insert_rotations),
      CU_TEST_INFO_NULL};

  CU_SuiteInfo suites[] = {{.pName = "BinaryTree_Create",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = create_tests},
                           {.pName = "BinaryTree_Insert",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = insert_tests},
                           {.pName = "BinaryTree_Delete",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = delete_tests},
                           {.pName = "BinaryTree_Enumerate",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = enumerate_tests},
                           {.pName = "BinaryTree_Search",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = search_tests},
                           {.pName = "BinaryTree_Min and BinaryTree_Max",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = min_max_tests},
                           {.pName = "BinaryTree_Predecessor",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = predecessor_tests},
                           {.pName = "BinaryTree_Successor",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = Successor_tests},
                           {.pName = "BinaryTree_Select",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = select_tests},
                           {.pName = "BinaryTree_Rank",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = rank_tests},
                           {.pName = "BinaryTree_Destroy",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = destroy_tests},
                           {.pName = "BinaryTree Common",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = common_tests},
                           {.pName = "BinaryTree Rotations",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = rotate_tests},
                           {.pName = "RedBlackTree_Insert",
                            .pInitFunc = noop,
                            .pCleanupFunc = noop,
                            .pTests = red_black_insert_tests},
                           CU_SUITE_INFO_NULL};

  return CU_register_suites(suites);
}
