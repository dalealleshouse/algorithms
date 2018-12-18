#include <limits.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"
#include "TreeTestHelpers.h"

size_t NodeCount(BinaryTreeNode* node)
{
    if (node == NULL)
        return 0;

    size_t nc = NodeCount(node->left) + NodeCount(node->right) + 1;
    if (nc != node->size) {
        int node_id = *(int*)node->payload;
        printf("invalid size: id=%d expected=%zu, actual=%zu\n", node_id, nc,
            node->size);
        CU_FAIL();
    }
    return nc;
}

void TreeSizeEqualsNodeCount(BinaryTree* tree)
{
    CU_ASSERT_EQUAL(tree->n, NodeCount(tree->root));
}

void NodeIsValid(BinaryTreeNode* node)
{
    if (node == NULL)
        return;

    int node_id = *(int*)node->payload;

    if (node->left != NULL) {
        int result = int_comparator(node->payload, node->left->payload);
        if (result < 0) {
            printf(
                "ASSERT FAILURE on node %d - left item is greater than current",
                node_id);
            CU_FAIL();
        }
    }

    if (node->right != NULL) {
        int result = int_comparator(node->payload, node->right->payload);
        if (result > 0) {
            printf(
                "ASSERT FAILURE on node %d - right item is less than current",
                node_id);
            CU_FAIL();
        }
    }
}

void TreePropertyHolds(BinaryTreeNode* node)
{
    if (node == NULL)
        return;

    TreePropertyHolds(node->left);
    NodeIsValid(node);
    TreePropertyHolds(node->right);
}

void TreeIsValid(BinaryTree* tree, size_t n)
{
    CU_ASSERT_PTR_NOT_NULL(tree);

    if (n == 0) {
        CU_ASSERT_PTR_NULL(tree->root);
    } else {
        CU_ASSERT_EQUAL(tree->n, tree->root->size);
    }

    CU_ASSERT_EQUAL(n, tree->n);
    TreeSizeEqualsNodeCount(tree);
    TreePropertyHolds(tree->root);
}

BinaryTree* ManuallyGenerateValidTree()
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
