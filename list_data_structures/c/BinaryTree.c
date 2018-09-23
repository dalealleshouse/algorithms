#include <stdlib.h>

#include "MemAllocMock.h"
#include "BinaryTree.h"

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

ListOpResult BinaryTree_Insert(BinaryTree* self, void* item)
{
    if (item == NULL)
        return ListOp_NullParamter;

    BinaryTreeNode* node = BinaryTree_CreateNode();
    if (node == NULL)
        return ListOp_FailedMalloc;

    node->item = item;

    if (self->root == NULL)
        self->root = node;

    return ListOp_Success;
}

void BinaryTree_Destroy(BinaryTree* self)
{
    if (self != NULL)
        free(self->root);

    free(self);
}
