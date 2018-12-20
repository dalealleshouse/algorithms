#include <stdint.h>
#include <stdlib.h>

#include "BinaryTree.h"
#include "include/MemAllocMock.h"

BinaryTreeNode sentinel = { NULL, NULL, NULL, NULL, 0, BLACK };

static void delete(BinaryTreeNode**);
static BinaryTreeNode* max(BinaryTreeNode*);

static void tree_node_destroy(BinaryTreeNode* node, freer freer)
{
    if (node == &sentinel)
        return;

    if (freer != NULL)
        freer(node->payload);

    free(node);
}

static BinaryTreeNode* tree_node_create(void* payload)
{
    BinaryTreeNode* node = calloc(sizeof(BinaryTreeNode), 1);
    if (node == NULL)
        return NULL;

    node->parent = &sentinel;
    node->left = &sentinel;
    node->right = &sentinel;
    node->payload = payload;
    node->size = 1;
    node->color = BLACK;

    return node;
}

static BinaryTreeNode* traverse(
    BinaryTreeNode* node, const void* search_for, comparator comparator)
{
    if (node == &sentinel)
        return NULL;

    int result = comparator(search_for, node->payload);

    if (result == 0)
        return node;

    if (result < 0)
        return traverse(node->left, search_for, comparator);

    return traverse(node->right, search_for, comparator);
}

static void insert_node(
    BinaryTreeNode* root, BinaryTreeNode* new, comparator comparator)
{
    root->size++;
    int result = comparator(new->payload, root->payload);

    if (result <= 0) {
        if (root->left == &sentinel) {
            root->left = new;
            new->parent = root;
        } else {
            insert_node(root->left, new, comparator);
        }
        return;
    }

    if (root->right == &sentinel) {
        root->right = new;
        new->parent = root;
    } else {
        insert_node(root->right, new, comparator);
    }
}

static void decrement_size(BinaryTreeNode* node)
{
    if (node == &sentinel)
        return;

    decrement_size(node->parent);
    node->size--;
}

static void delete_leaf(BinaryTreeNode** doomed)
{
    BinaryTreeNode* node = *doomed;

    decrement_size(node);

    tree_node_destroy(node, NULL);
    *doomed = &sentinel;
}

static void delete_degree_one(BinaryTreeNode** doomed)
{
    BinaryTreeNode* node = *doomed;

    decrement_size(node);
    BinaryTreeNode* child
        = (node->left != &sentinel) ? node->left : node->right;
    child->parent = node->parent;
    tree_node_destroy(node, NULL);
    *doomed = child;
}

static BinaryTreeNode** find_parent_pointer(BinaryTreeNode* node)
{
    BinaryTreeNode* parent = node->parent;

    if (parent->left == node)
        return &parent->left;

    return &parent->right;
}

static void delete_degree_two(BinaryTreeNode** doomed)
{
    BinaryTreeNode* node = *doomed;

    BinaryTreeNode* largest_left = max(node->left);

    void* new_value = largest_left->payload;
    node->payload = new_value;
    BinaryTreeNode** doomed_p = find_parent_pointer(largest_left);
    delete(doomed_p);
}

static size_t degree(BinaryTreeNode* node)
{
    return (node->left != &sentinel) + (node->right != &sentinel);
}

static void delete(BinaryTreeNode** doomed)
{
    BinaryTreeNode* node = *doomed;

    size_t deg = degree(node);

    switch (deg) {
    case 0:
        delete_leaf(doomed);
        break;
    case 1:
        delete_degree_one(doomed);
        break;
    case 2:
        delete_degree_two(doomed);
        break;
    }
}

static void enumerate(BinaryTreeNode* node, item_handler payload_handler)
{
    if (node == &sentinel)
        return;

    enumerate(node->left, payload_handler);
    payload_handler(node->payload);
    enumerate(node->right, payload_handler);
}

static BinaryTreeNode* min(BinaryTreeNode* root)
{
    if (root->left == &sentinel)
        return root;

    return min(root->left);
}

static BinaryTreeNode* max(BinaryTreeNode* root)
{
    if (root->right == &sentinel)
        return root;

    return max(root->right);
}

static const BinaryTreeNode* parent_less_than(
    const BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    if (root == &sentinel)
        return NULL;

    int result = comparator(search_for, root->payload);

    if (result > 0)
        return root;

    return parent_less_than(root->parent, search_for, comparator);
}

static const BinaryTreeNode* predecessor(
    BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    const BinaryTreeNode* node = traverse(root, search_for, comparator);
    if (node == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NotFound);
        return NULL;
    }

    if (node->left != &sentinel)
        return max(node->left);

    return parent_less_than(node->parent, search_for, comparator);
}

static const BinaryTreeNode* parent_greater_than(
    const BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    if (root == &sentinel)
        return NULL;

    int result = comparator(search_for, root->payload);

    if (result < 0)
        return root;

    return parent_greater_than(root->parent, search_for, comparator);
}

static const BinaryTreeNode* successor(
    BinaryTreeNode* root, const void* search_for, comparator comparator)
{
    const BinaryTreeNode* node = traverse(root, search_for, comparator);
    if (node == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NotFound);
        return NULL;
    }

    if (node->right != &sentinel)
        return min(node->right);

    return parent_greater_than(node->parent, search_for, comparator);
}

static size_t node_size(const BinaryTreeNode* node)
{
    return (node == 0) ? 0 : node->size;
}

static const BinaryTreeNode* select(
    const BinaryTreeNode* root, const size_t index)
{
    size_t left = node_size(root->left);

    if (left == index)
        return root;

    if (index < left)
        return select(root->left, index);

    return select(root->right, index - left - 1);
}

static size_t rank(const BinaryTreeNode* root, const void* payload,
    comparator comparator, size_t offset)
{
    if (root == &sentinel)
        return RANK_ERROR;

    int result = comparator(payload, root->payload);

    size_t left = node_size(root->left);
    if (result == 0)
        return left + offset;

    if (result < 0)
        return rank(root->left, payload, comparator, offset);

    return rank(root->right, payload, comparator, offset + left + 1);
}

static void balance(BinaryTreeNode* node) {}

static void free_nodes(BinaryTreeNode* node, freer freer)
{
    if (node == &sentinel || node == NULL)
        return;

    free_nodes(node->left, freer);
    free_nodes(node->right, freer);

    tree_node_destroy(node, freer);
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
    tree->root = &sentinel;
    return tree;
}

ListOpResult BinaryTree_Insert(BinaryTree* self, void* payload)
{
    if (self == NULL || payload == NULL)
        return ListOp_NullParameter;

    BinaryTreeNode* node = tree_node_create(payload);
    if (node == NULL)
        return ListOp_FailedMalloc;

    if (self->root == &sentinel)
        self->root = node;
    else
        insert_node(self->root, node, self->comparator);

    self->n++;
    return ListOp_Success;
}

void* BinaryTree_Delete(BinaryTree* self, void* payload)
{
    if (self == NULL || payload == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return NULL;
    }

    BinaryTreeNode* doomed = traverse(self->root, payload, self->comparator);
    if (doomed == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NotFound);
        return NULL;
    }

    void* result = doomed->payload;

    if (doomed == self->root)
        delete(&self->root);
    else
        delete(find_parent_pointer(doomed));

    self->n--;
    return result;
}

void* BinaryTree_Search(const BinaryTree* self, const void* payload)
{
    if (self == NULL || payload == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return NULL;
    }

    BinaryTreeNode* found = traverse(self->root, payload, self->comparator);
    if (found == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NotFound);
        return NULL;
    }

    return found->payload;
}

ListOpResult BinaryTree_Enumerate(
    const BinaryTree* self, item_handler payload_handler)
{
    if (self == NULL || payload_handler == NULL)
        return ListOp_NullParameter;

    enumerate(self->root, payload_handler);
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

    BinaryTreeNode* node = min(self->root);
    return node->payload;
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

    BinaryTreeNode* node = max(self->root);
    return node->payload;
}

void* BinaryTree_Predecessor(const BinaryTree* self, const void* payload)
{
    if (self == NULL || payload == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return NULL;
    }

    const BinaryTreeNode* pred
        = predecessor(self->root, payload, self->comparator);
    if (pred == NULL)
        return NULL;

    return pred->payload;
}

void* BinaryTree_Successor(const BinaryTree* self, const void* payload)
{
    if (self == NULL || payload == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return NULL;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return NULL;
    }

    const BinaryTreeNode* succ
        = successor(self->root, payload, self->comparator);
    if (succ == NULL)
        return NULL;

    return succ->payload;
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

    return select(self->root, index)->payload;
}

size_t BinaryTree_Rank(const BinaryTree* self, const void* payload)
{
    if (self == NULL || payload == NULL) {
        LIST_ERROR("Binary Tree", ListOp_NullParameter);
        return RANK_ERROR;
    }

    if (self->n == 0) {
        LIST_ERROR("Binary Tree", ListOp_EmptyList);
        return RANK_ERROR;
    }

    size_t result = rank(self->root, payload, self->comparator, 0);

    if (result == RANK_ERROR)
        LIST_ERROR("Binary Tree", ListOp_NotFound);

    return result;
}

void BinaryTree_Destroy(BinaryTree* self, freer freer)
{
    if (self == NULL)
        return;

    free_nodes(self->root, freer);
    free(self);
}

ListOpResult RedBlackTree_Insert(BinaryTree* self, void* payload)
{
    if (self == NULL || payload == NULL)
        return ListOp_NullParameter;

    BinaryTreeNode* node = tree_node_create(payload);
    if (node == NULL)
        return ListOp_FailedMalloc;

    if (self->root == &sentinel)
        self->root = node;
    else {
        insert_node(self->root, node, self->comparator);
        node->color = RED;
    }

    self->n++;
    balance(node);
    return ListOp_Success;
}
