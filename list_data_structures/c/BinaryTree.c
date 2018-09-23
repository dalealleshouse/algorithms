#include <stdio.h>
#include <stdlib.h>

#include "BinaryTree.h"
#include "MemAllocMock.h"

static BinaryTreeNode* BinaryTree_CreateNode()
{
    return calloc(sizeof(BinaryTreeNode), 1);
}

BinaryTree* BinaryTree_Create(comparator comparator)
{
    if (comparator == NULL)
        return NULL;

    BinaryTree* self = calloc(sizeof(BinaryTree), 1);
    if (self == NULL)
        return NULL;

    self->comparator = comparator;

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
    if (item == NULL)
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

void BinaryTree_Destroy(BinaryTree* self)
{
    if (self != NULL)
        free(self->root);

    free(self);
}
