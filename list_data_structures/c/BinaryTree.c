#include <stdint.h>
#include <stdlib.h>

#include "BinaryTree.h"
#include "include/MemAllocMock.h"

BinaryTree* BinaryTree_Create(comparator comparator)
{
    if (comparator == NULL) {
        LIST_ERROR("Search Tree", ListOp_NullParameter);
        return NULL;
    }

    BinaryTree* tree = calloc(sizeof(BinaryTree), 1);
    if (tree == NULL) {
        LIST_ERROR("Search Tree", ListOp_FailedMalloc);
        return NULL;
    }

    tree->comparator = comparator;
    return tree;
}

static BinaryTreeNode* CreateNode(void* item)
{
    BinaryTreeNode* node = calloc(sizeof(BinaryTreeNode), 1);
    if (node == NULL)
        return NULL;

    node->item = item;
    node->size = 1;

    return node;
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
    } else {
        if (root->right == NULL) {
            root->right = new;
            new->parent = root;
        } else {
            InsertNode(root->right, new, comparator);
        }
    }
}

ListOpResult BinaryTree_Insert(BinaryTree* self, void* item)
{
    if (self == NULL || item == NULL)
        return ListOp_NullParameter;

    BinaryTreeNode* node = CreateNode(item);
    if (node == NULL)
        return ListOp_FailedMalloc;

    if (self->root == NULL)
        self->root = node;
    else
        InsertNode(self->root, node, self->comparator);

    self->n++;
    return ListOp_Success;
}

ListOpResult BinaryTree_Delete(BinaryTree* self, void* item)
{
    return ListOp_Success;
}

static void Enumerate(BinaryTreeNode* node, item_handler item_handler)
{
    if (node == NULL)
        return;

    Enumerate(node->left, item_handler);
    item_handler(node->item);
    Enumerate(node->right, item_handler);
}

ListOpResult BinaryTree_Enumerate(
    const BinaryTree* self, item_handler item_handler)
{
    if (self == NULL || item_handler == NULL)
        return ListOp_NullParameter;

    Enumerate(self->root, item_handler);
    return ListOp_Success;
}

static void* Search(
    const BinaryTreeNode* node, const void* search_for, comparator comparator)
{
    if (node == NULL)
        return NULL;

    int result = comparator(search_for, node->item);

    if (result == 0)
        return node->item;

    if (result < 0)
        return Search(node->left, search_for, comparator);

    return Search(node->right, search_for, comparator);
}

void* BinaryTree_Search(const BinaryTree* self, const void* item)
{
    if (self == NULL || item == NULL) {
        LIST_ERROR("Search Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Search Tree", ListOp_EmptyList);
        return NULL;
    }

    void* found = Search(self->root, item, self->comparator);
    if (found == NULL)
        LIST_ERROR("Search Tree", ListOp_NotFound);

    return found;
}

static void* Min(BinaryTreeNode* root)
{
    if (root->left == NULL)
        return root->item;

    return Min(root->left);
}

void* BinaryTree_Min(const BinaryTree* self)
{
    if (self == NULL) {
        LIST_ERROR("Search Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Search Tree", ListOp_EmptyList);
        return NULL;
    }

    return Min(self->root);
}

static void* Max(BinaryTreeNode* root)
{
    if (root->right == NULL)
        return root->item;

    return Max(root->right);
}

void* BinaryTree_Max(const BinaryTree* self)
{
    if (self == NULL) {
        LIST_ERROR("Search Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Search Tree", ListOp_EmptyList);
        return NULL;
    }

    return Max(self->root);
}

static void* ParentLessThan(
    const BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    if (root == NULL)
        return NULL;

    int result = comparator(search_for, root->item);

    if (result > 0)
        return root->item;

    return ParentLessThan(root->parent, search_for, comparator);
}

static void* Predecessor(
    const BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    if (root == NULL) {
        LIST_ERROR("Search Tree", ListOp_NotFound);
        return NULL;
    }

    int result = comparator(search_for, root->item);

    if (result == 0) {
        if (root->left != NULL)
            return Max(root->left);

        return ParentLessThan(root->parent, search_for, comparator);
    }

    if (result < 0)
        return Predecessor(root->left, search_for, comparator);

    return Predecessor(root->right, search_for, comparator);
}

void* BinaryTree_Predecessor(const BinaryTree* self, const void* item)
{
    if (self == NULL || item == NULL) {
        LIST_ERROR("Search Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Search Tree", ListOp_EmptyList);
        return NULL;
    }

    void* pred = Predecessor(self->root, item, self->comparator);
    return pred;
}

static void* ParentGreaterThan(
    const BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    if (root == NULL)
        return NULL;

    int result = comparator(search_for, root->item);

    if (result < 0)
        return root->item;

    return ParentGreaterThan(root->parent, search_for, comparator);
}

static void* Successor(
    const BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    if (root == NULL) {
        LIST_ERROR("Search Tree", ListOp_NotFound);
        return NULL;
    }

    int result = comparator(search_for, root->item);

    if (result == 0) {
        if (root->right != NULL)
            return Min(root->right);

        return ParentGreaterThan(root->parent, search_for, comparator);
    }

    if (result < 0)
        return Successor(root->left, search_for, comparator);

    return Successor(root->right, search_for, comparator);
}

void* BinaryTree_Successor(const BinaryTree* self, const void* item)
{
    if (self == NULL || item == NULL) {
        LIST_ERROR("Search Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Search Tree", ListOp_EmptyList);
        return NULL;
    }

    void* succ = Successor(self->root, item, self->comparator);
    return succ;
}

static size_t RootIndex(const BinaryTreeNode* root)
{
    size_t i = root->size - ((root->right == NULL) ? 0 : root->right->size);
    printf("id=%d, index=%zu\n", *(int*)root->item, i);
    return i;
}

static size_t NodeSize(const BinaryTreeNode* node)
{
    return (node == 0) ? 0 : node->size;
}

static void* Select(const BinaryTreeNode* root, const size_t index)
{
    size_t left = NodeSize(root->left);

    if (left == index)
        return root->item;

    if (index < left)
        return Select(root->left, index);

    return Select(root->right, index - left - 1);
}

void* BinaryTree_Select(const BinaryTree* self, const size_t index)
{
    if (self == NULL) {
        LIST_ERROR("Search Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Search Tree", ListOp_EmptyList);
        return NULL;
    }

    if (index >= self->n) {
        LIST_ERROR("Search Tree", ListOp_InvalidIndex);
        return NULL;
    }

    return Select(self->root, index);
}

size_t Rank(const BinaryTreeNode* root, const void* item, comparator comparator,
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

size_t BinaryTree_Rank(const BinaryTree* self, const void* item)
{
    if (self == NULL || item == NULL) {
        LIST_ERROR("Search Tree", ListOp_NullParameter);
        return RANK_ERROR;
    }

    if (self->n == 0) {
        LIST_ERROR("Search Tree", ListOp_EmptyList);
        return RANK_ERROR;
    }

    size_t result = Rank(self->root, item, self->comparator, 0);

    if (result == RANK_ERROR)
        LIST_ERROR("Search Tree", ListOp_NotFound);

    return result;
}

static void FreeNodes(BinaryTreeNode* node, freer freer)
{
    if (node == NULL)
        return;

    FreeNodes(node->left, freer);
    FreeNodes(node->right, freer);

    if (freer != NULL)
        freer(node->item);

    free(node);
}

void BinaryTree_Destroy(BinaryTree* self, freer freer)
{
    FreeNodes(self->root, freer);
    free(self);
}
