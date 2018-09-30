#include <stdio.h>
#include <stdlib.h>

#include "BinaryTree.h"
#include "MemAllocMock.h"

static BinaryTreeNode* BinaryTree_FindNode(
    const BinaryTree* self, BinaryTreeNode* node, const void* item)
{
    if (node == NULL)
        return NULL;

    int comp_result = self->comparator(item, node->item);

    if (comp_result == 0)
        return node;
    else if (comp_result < 0)
        return BinaryTree_FindNode(self, node->left, item);
    else
        return BinaryTree_FindNode(self, node->right, item);
}

static BinaryTreeNode* BinaryTree_CreateNode()
{
    return calloc(sizeof(BinaryTreeNode), 1);
}

static void BinaryTree_FreeNode(BinaryTreeNode* node, freer freer)
{
    if (freer != NULL)
        freer(node->item);

    free(node);
}

static void BinaryTree_FreeNodeAndChildren(BinaryTreeNode* node, freer freer)
{
    if (node == NULL)
        return;

    if (node->left != NULL)
        BinaryTree_FreeNodeAndChildren(node->left, freer);

    if (node->right != NULL)
        BinaryTree_FreeNodeAndChildren(node->right, freer);

    BinaryTree_FreeNode(node, freer);
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
    BinaryTree* self, BinaryTreeNode* root, BinaryTreeNode* item)
{
    if (self->root == NULL) {
        self->root = item;
        return;
    }

    int comp_result = self->comparator(item->item, root->item);

    if (comp_result < 0) {
        if (root->left == NULL)
            root->left = item;
        else
            BinaryTree_ExpandTree(self, root->left, item);
    } else {
        if (root->right == NULL)
            root->right = item;
        else
            BinaryTree_ExpandTree(self, root->right, item);
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

    BinaryTree_ExpandTree(self, self->root, node);
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

        BinaryTree_FreeNode(node, self->freer);
        *root = NULL;

        if (left != NULL)
            BinaryTree_ExpandTree(self, self->root, left);

        if (right != NULL)
            BinaryTree_ExpandTree(self, self->root, right);

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

    /* BinaryTreeNode* node = BinaryTree_FindNode(self, self->root, item); */

    /* if(node == NULL) */
    /*     return ListOp_NotFound; */

    /* if(node->left == NULL && node->right == NULL) */
    /*     BinaryTree_FreeNode(node, self->freer); */

    /* return ListOp_Success; */

    return BinaryTree_DeleteNode(self, &self->root, item);
}

void* BinaryTree_Search(const BinaryTree* self, const void* item)
{
    if (self == NULL || item == NULL)
        return NULL;

    BinaryTreeNode* node = BinaryTree_FindNode(self, self->root, item);

    if (node == NULL)
        return NULL;

    return node->item;
}

static void BinaryTree_HandleNodes(BinaryTreeNode* node, item_handler handler)
{
    if (node == NULL)
        return;

    BinaryTree_HandleNodes(node->left, handler);
    handler(node->item);
    BinaryTree_HandleNodes(node->right, handler);
}

ListOpResult BinaryTree_Enumerate(const BinaryTree* self, item_handler handler)
{
    if (self == NULL || handler == NULL)
        return ListOp_NullParameter;

    BinaryTree_HandleNodes(self->root, handler);

    return ListOp_Success;
}

void BinaryTree_Destroy(BinaryTree* self)
{
    if (self != NULL)
        BinaryTree_FreeNodeAndChildren(self->root, self->freer);

    free(self);
}
