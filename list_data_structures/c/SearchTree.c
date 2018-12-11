#include <stdlib.h>

#include "SearchTree.h"
#include "include/MemAllocMock.h"

SearchTree* SearchTree_Create(comparator comparator)
{
    if (comparator == NULL) {
        LIST_ERROR("Search Tree", ListOp_NullParameter);
        return NULL;
    }

    SearchTree* tree = calloc(sizeof(SearchTree), 1);
    if (tree == NULL) {
        LIST_ERROR("Search Tree", ListOp_FailedMalloc);
        return NULL;
    }

    tree->comparator = comparator;
    return tree;
}

static SearchTreeNode* CreateNode(void* item)
{
    SearchTreeNode* node = calloc(sizeof(SearchTreeNode), 1);
    if (node == NULL)
        return NULL;

    node->item = item;
    node->size = 1;

    return node;
}

static void InsertNode(
    SearchTreeNode* root, SearchTreeNode* new, comparator comparator)
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

ListOpResult SearchTree_Insert(SearchTree* self, void* item)
{
    if (self == NULL || item == NULL)
        return ListOp_NullParameter;

    SearchTreeNode* node = CreateNode(item);
    if (node == NULL)
        return ListOp_FailedMalloc;

    if (self->root == NULL)
        self->root = node;
    else
        InsertNode(self->root, node, self->comparator);

    self->n++;
    return ListOp_Success;
}

ListOpResult SearchTree_Delete(SearchTree* self, void* item)
{
    return ListOp_Success;
}

static void Enumerate(SearchTreeNode* node, item_handler item_handler)
{
    if (node == NULL)
        return;

    Enumerate(node->left, item_handler);
    item_handler(node->item);
    Enumerate(node->right, item_handler);
}

ListOpResult SearchTree_Enumerate(
    const SearchTree* self, item_handler item_handler)
{
    if (self == NULL || item_handler == NULL)
        return ListOp_NullParameter;

    Enumerate(self->root, item_handler);
    return ListOp_Success;
}

static void* Search(
    const SearchTreeNode* node, const void* search_for, comparator comparator)
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

void* SearchTree_Search(const SearchTree* self, const void* item)
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

static void* Min(SearchTreeNode* root)
{
    if (root->left == NULL)
        return root->item;

    return Min(root->left);
}

void* SearchTree_Min(const SearchTree* self)
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

static void* Max(SearchTreeNode* root)
{
    if (root->right == NULL)
        return root->item;

    return Max(root->right);
}

void* SearchTree_Max(const SearchTree* self)
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
    const SearchTreeNode* root, const void* search_for, comparator comparator)
{
    if (root == NULL)
        return NULL;

    int result = comparator(search_for, root->item);

    if (result > 0)
        return root->item;

    return ParentLessThan(root->parent, search_for, comparator);
}

static void* Predecessor(
    const SearchTreeNode* root, const void* search_for, comparator comparator)
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

void* SearchTree_Predecessor(const SearchTree* self, const void* item)
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
    const SearchTreeNode* root, const void* search_for, comparator comparator)
{
    if (root == NULL)
        return NULL;

    int result = comparator(search_for, root->item);

    if (result < 0)
        return root->item;

    return ParentGreaterThan(root->parent, search_for, comparator);
}

static void* Successor(
    const SearchTreeNode* root, const void* search_for, comparator comparator)
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

void* SearchTree_Successor(const SearchTree* self, const void* item)
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

void* SearchTree_Select(const SearchTree* self, const size_t index)
{
    return NULL;
}

size_t SearchTree_Rank(const SearchTree* self, const void* item) { return 0; }

static void FreeNodes(SearchTreeNode* node, freer freer)
{
    if (node == NULL)
        return;

    FreeNodes(node->left, freer);
    FreeNodes(node->right, freer);

    if (freer != NULL)
        freer(node->item);

    free(node);
}

void SearchTree_Destroy(SearchTree* self, freer freer)
{
    FreeNodes(self->root, freer);
    free(self);
}
