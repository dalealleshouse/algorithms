#include <stdint.h>
#include <stdlib.h>

#include "BinaryTree.h"
#include "include/MemAllocMock.h"

static void Delete(BinaryTreeNode**);
static BinaryTreeNode* Max(BinaryTreeNode*);

static void TreeNodeDestroy(BinaryTreeNode* node, freer freer)
{
    if (node == NULL)
        return;

    if (freer != NULL)
        freer(node->item);

    free(node);
}

static BinaryTreeNode* TreeNodeCreate(void* item)
{
    BinaryTreeNode* node = calloc(sizeof(BinaryTreeNode), 1);
    if (node == NULL)
        return NULL;

    node->item = item;
    node->size = 1;

    return node;
}

static BinaryTreeNode* Traverse(
    BinaryTreeNode* node, const void* search_for, comparator comparator)
{
    if (node == NULL)
        return NULL;

    int result = comparator(search_for, node->item);

    if (result == 0)
        return node;

    if (result < 0)
        return Traverse(node->left, search_for, comparator);

    return Traverse(node->right, search_for, comparator);
}

static void InsertNode(
    BinaryTreeNode* root, BinaryTreeNode* new, comparator comparator)
{
    root->size++;
    int result = comparator(new->item, root->item);

    if (result <= 0) {
        if (root->left == NULL) {
            root->left = new;
            new->parent = root;
        } else {
            InsertNode(root->left, new, comparator);
        }
        return;
    }

    if (root->right == NULL) {
        root->right = new;
        new->parent = root;
    } else {
        InsertNode(root->right, new, comparator);
    }
}

static void DecrementSize(BinaryTreeNode* node)
{
    if (node == NULL)
        return;

    DecrementSize(node->parent);
    node->size--;
}

static void DeleteLeaf(BinaryTreeNode** doomed)
{
    BinaryTreeNode* node = *doomed;

    DecrementSize(node);

    TreeNodeDestroy(node, NULL);
    *doomed = NULL;
}

static void DeleteDegreeOne(BinaryTreeNode** doomed)
{
    BinaryTreeNode* node = *doomed;

    DecrementSize(node);
    BinaryTreeNode* child = (node->left != NULL) ? node->left : node->right;
    child->parent = node->parent;
    TreeNodeDestroy(node, NULL);
    *doomed = child;
}

static BinaryTreeNode** FindParentPointer(BinaryTreeNode* node)
{
    BinaryTreeNode* parent = node->parent;

    if (parent->left == node)
        return &parent->left;

    return &parent->right;
}

static void DeleteDegreeTwo(BinaryTreeNode** doomed)
{
    BinaryTreeNode* node = *doomed;

    BinaryTreeNode* largest_left = Max(node->left);

    void* new_value = largest_left->item;
    node->item = new_value;
    BinaryTreeNode** doomed_p = FindParentPointer(largest_left);
    Delete(doomed_p);
}

static size_t Degree(BinaryTreeNode* node)
{
    return (node->left != NULL) + (node->right != NULL);
}

static void Delete(BinaryTreeNode** doomed)
{
    BinaryTreeNode* node = *doomed;

    size_t degree = Degree(node);

    switch (degree) {
    case 0:
        DeleteLeaf(doomed);
        break;
    case 1:
        DeleteDegreeOne(doomed);
        break;
    case 2:
        DeleteDegreeTwo(doomed);
        break;
    }
}

static void Enumerate(BinaryTreeNode* node, item_handler item_handler)
{
    if (node == NULL)
        return;

    Enumerate(node->left, item_handler);
    item_handler(node->item);
    Enumerate(node->right, item_handler);
}

static BinaryTreeNode* Min(BinaryTreeNode* root)
{
    if (root->left == NULL)
        return root;

    return Min(root->left);
}

static BinaryTreeNode* Max(BinaryTreeNode* root)
{
    if (root->right == NULL)
        return root;

    return Max(root->right);
}

static const BinaryTreeNode* ParentLessThan(
    const BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    if (root == NULL)
        return NULL;

    int result = comparator(search_for, root->item);

    if (result > 0)
        return root;

    return ParentLessThan(root->parent, search_for, comparator);
}

static const BinaryTreeNode* Predecessor(
    BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    const BinaryTreeNode* node = Traverse(root, search_for, comparator);
    if (node == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NotFound);
        return NULL;
    }

    if (node->left != NULL)
        return Max(node->left);

    return ParentLessThan(node->parent, search_for, comparator);
}

static const BinaryTreeNode* ParentGreaterThan(
    const BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    if (root == NULL)
        return NULL;

    int result = comparator(search_for, root->item);

    if (result < 0)
        return root;

    return ParentGreaterThan(root->parent, search_for, comparator);
}

static const BinaryTreeNode* Successor(
    BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    const BinaryTreeNode* node = Traverse(root, search_for, comparator);
    if (node == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NotFound);
        return NULL;
    }

    if (node->right != NULL)
        return Min(node->right);

    return ParentGreaterThan(node->parent, search_for, comparator);
}

static size_t NodeSize(const BinaryTreeNode* node)
{
    return (node == 0) ? 0 : node->size;
}

static const BinaryTreeNode* Select(
    const BinaryTreeNode* root, const size_t index)
{
    size_t left = NodeSize(root->left);

    if (left == index)
        return root;

    if (index < left)
        return Select(root->left, index);

    return Select(root->right, index - left - 1);
}

static size_t Rank(const BinaryTreeNode* root, const void* item, comparator comparator,
    size_t offset)
{
    if (root == NULL)
        return RANK_ERROR;

    int result = comparator(item, root->item);

    size_t left = NodeSize(root->left);
    if (result == 0)
        return left + offset;

    if (result < 0)
        return Rank(root->left, item, comparator, offset);

    return Rank(root->right, item, comparator, offset + left + 1);
}

static void FreeNodes(BinaryTreeNode* node, freer freer)
{
    if (node == NULL)
        return;

    FreeNodes(node->left, freer);
    FreeNodes(node->right, freer);

    TreeNodeDestroy(node, freer);
}

BinaryTree* BinaryTree_Create(comparator comparator)
{
    if (comparator == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    BinaryTree* tree = calloc(sizeof(BinaryTree), 1);
    if (tree == NULL) {
        LIST_ERROR("Binary Tree", ListOp_FailedMalloc);
        return NULL;
    }

    tree->comparator = comparator;
    return tree;
}

ListOpResult BinaryTree_Insert(BinaryTree* self, void* item)
{
    if (self == NULL || item == NULL)
        return ListOp_NullParameter;

    BinaryTreeNode* node = TreeNodeCreate(item);
    if (node == NULL)
        return ListOp_FailedMalloc;

    if (self->root == NULL)
        self->root = node;
    else
        InsertNode(self->root, node, self->comparator);

    self->n++;
    return ListOp_Success;
}

void* BinaryTree_Delete(BinaryTree* self, void* item)
{
    if (self == NULL || item == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return NULL;
    }

    BinaryTreeNode* doomed = Traverse(self->root, item, self->comparator);
    if (doomed == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NotFound);
        return NULL;
    }

    void* result = doomed->item;

    if (doomed == self->root)
        Delete(&self->root);
    else
        Delete(FindParentPointer(doomed));

    self->n--;
    return result;
}

void* BinaryTree_Search(const BinaryTree* self, const void* item)
{
    if (self == NULL || item == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return NULL;
    }

    BinaryTreeNode* found = Traverse(self->root, item, self->comparator);
    if (found == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NotFound);
        return NULL;
    }

    return found->item;
}

ListOpResult BinaryTree_Enumerate(
    const BinaryTree* self, item_handler item_handler)
{
    if (self == NULL || item_handler == NULL)
        return ListOp_NullParameter;

    Enumerate(self->root, item_handler);
    return ListOp_Success;
}

void* BinaryTree_Min(const BinaryTree* self)
{
    if (self == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return NULL;
    }

    BinaryTreeNode* node = Min(self->root);
    return node->item;
}

void* BinaryTree_Max(const BinaryTree* self)
{
    if (self == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return NULL;
    }

    BinaryTreeNode* node = Max(self->root);
    return node->item;
}

void* BinaryTree_Predecessor(const BinaryTree* self, const void* item)
{
    if (self == NULL || item == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return NULL;
    }

    const BinaryTreeNode* pred
        = Predecessor(self->root, item, self->comparator);
    if (pred == NULL)
        return NULL;

    return pred->item;
}

void* BinaryTree_Successor(const BinaryTree* self, const void* item)
{
    if (self == NULL || item == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return NULL;
    }

    const BinaryTreeNode* succ = Successor(self->root, item, self->comparator);
    if (succ == NULL)
        return NULL;

    return succ->item;
}

void* BinaryTree_Select(const BinaryTree* self, const size_t index)
{
    if (self == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return NULL;
    }

    if (index >= self->n) {
        LIST_ERROR("Binary Tree", ListOp_InvalidIndex);
        return NULL;
    }

    return Select(self->root, index)->item;
}

size_t BinaryTree_Rank(const BinaryTree* self, const void* item)
{
    if (self == NULL || item == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return RANK_ERROR;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return RANK_ERROR;
    }

    size_t result = Rank(self->root, item, self->comparator, 0);

    if (result == RANK_ERROR)
        LIST_ERROR("Binary Tree", ListOp_NotFound);

    return result;
}

void BinaryTree_Destroy(BinaryTree* self, freer freer)
{
    if (self == NULL)
        return;

    FreeNodes(self->root, freer);
    free(self);
}
