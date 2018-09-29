#include <stdio.h>
#include <stdlib.h>

#include "BinaryTree.h"
#include "MemAllocMock.h"

static BinaryTreeNode* BinaryTree_CreateNode()
{
    return calloc(sizeof(BinaryTreeNode), 1);
}

BinaryTree* BinaryTree_Create(comparator comparator, freer freer)
{
    if (comparator == NULL)
        return NULL;

    BinaryTree* self = calloc(sizeof(BinaryTree), 1);
    if (self == NULL)
        return NULL;

    self->comparator = comparator;
    self->freer = freer;

    return self;
}

static void BinaryTree_ExpandTree(
    BinaryTreeNode* root, BinaryTreeNode* item, comparator comparator)
{
    int comp_result = comparator(item->item, root->item);

    if (comp_result < 0) {
        if (root->left == NULL)
            root->left = item;
        else
            BinaryTree_ExpandTree(root->left, item, comparator);
    } else {
        if (root->right == NULL)
            root->right = item;
        else
            BinaryTree_ExpandTree(root->right, item, comparator);
    }
}

ListOpResult BinaryTree_Insert(BinaryTree* self, void* item)
{
    if (item == NULL || self == NULL)
        return ListOp_NullParameter;

    BinaryTreeNode* node = BinaryTree_CreateNode();
    if (node == NULL)
        return ListOp_FailedMalloc;

    node->item = item;

    if (self->root == NULL) {
        self->root = node;
        return ListOp_Success;
    }

    BinaryTree_ExpandTree(self->root, node, self->comparator);
    return ListOp_Success;
}

static ListOpResult BinaryTree_DeleteNode(
    BinaryTree* self, BinaryTreeNode** root, void* item)
{
    BinaryTreeNode* node = *root;

    if (node == NULL)
        return ListOp_NotFound;

    int comp_result = self->comparator(item, node->item);

    if (comp_result == 0) {
        BinaryTreeNode* left = node->left;
        BinaryTreeNode* right = node->right;

        if (self->freer != NULL)
            self->freer(node->item);

        free(node);
        *root = NULL;

        if (left != NULL)
            BinaryTree_ExpandTree(self->root, left, self->comparator);

        if (right != NULL)
            BinaryTree_ExpandTree(self->root, right, self->comparator);

        return ListOp_Success;
    }

    if (comp_result < 0)
        return BinaryTree_DeleteNode(self, &node->left, item);
    else
        return BinaryTree_DeleteNode(self, &node->right, item);
}

ListOpResult BinaryTree_Delete(BinaryTree* self, void* item)
{
    if (self == NULL || item == NULL)
        return ListOp_NullParameter;

    return BinaryTree_DeleteNode(self, &self->root, item);
}

static void BinaryTree_FreeNode(BinaryTreeNode* node)
{
    if (node == NULL)
        return;

    if (node->left != NULL)
        BinaryTree_FreeNode(node->left);

    if (node->right != NULL)
        BinaryTree_FreeNode(node->right);

    free(node);
}

void BinaryTree_Destroy(BinaryTree* self)
{
    if (self != NULL)
        BinaryTree_FreeNode(self->root);

    free(self);
}
