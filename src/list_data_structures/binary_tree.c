// Copyright 2020 Dale Alleshouse
#include "binary_tree.h"

#include <stdint.h>
#include <stdlib.h>

BinaryTreeNode NULL_NODE = {NULL, NULL, NULL, NULL, 0, BLACK};

static void delete (BinaryTreeNode**);
static BinaryTreeNode* Max(BinaryTreeNode*);

static void tree_node_destroy(BinaryTreeNode* node, freer freer) {
  if (node == &NULL_NODE) return;

  if (freer != NULL) freer(node->payload);

  free(node);
}

static BinaryTreeNode* tree_node_create(void* payload) {
  BinaryTreeNode* node = calloc(sizeof(BinaryTreeNode), 1);
  if (node == NULL) return NULL;

  node->parent = &NULL_NODE;
  node->left = &NULL_NODE;
  node->right = &NULL_NODE;
  node->payload = payload;
  node->size = 1;
  node->color = BLACK;

  return node;
}

static BinaryTreeNode* traverse(BinaryTreeNode* node, const void* search_for,
                                comparator comparator) {
  if (node == &NULL_NODE) return NULL;

  int result = comparator(search_for, node->payload);

  if (result == 0) return node;

  if (result < 0) return traverse(node->left, search_for, comparator);

  return traverse(node->right, search_for, comparator);
}

static void insert_node(BinaryTreeNode* root, BinaryTreeNode* new,
                        comparator comparator) {
  root->size++;
  int result = comparator(new->payload, root->payload);

  if (result <= 0) {
    if (root->left == &NULL_NODE) {
      root->left = new;
      new->parent = root;
    } else {
      insert_node(root->left, new, comparator);
    }
    return;
  }

  if (root->right == &NULL_NODE) {
    root->right = new;
    new->parent = root;
  } else {
    insert_node(root->right, new, comparator);
  }
}

static void decrement_size(BinaryTreeNode* node) {
  if (node == &NULL_NODE) return;

  decrement_size(node->parent);
  node->size--;
}

static void delete_leaf(BinaryTreeNode** doomed) {
  BinaryTreeNode* node = *doomed;

  decrement_size(node);

  tree_node_destroy(node, NULL);
  *doomed = &NULL_NODE;
}

static void delete_degree_one(BinaryTreeNode** doomed) {
  BinaryTreeNode* node = *doomed;

  decrement_size(node);
  BinaryTreeNode* child = (node->left != &NULL_NODE) ? node->left : node->right;
  child->parent = node->parent;
  tree_node_destroy(node, NULL);
  *doomed = child;
}

static BinaryTreeNode** find_parent_pointer(BinaryTreeNode* node) {
  BinaryTreeNode* parent = node->parent;

  if (parent->left == node) return &parent->left;

  return &parent->right;
}

static void delete_degree_two(BinaryTreeNode** doomed) {
  BinaryTreeNode* node = *doomed;

  BinaryTreeNode* largest_left = Max(node->left);

  void* new_value = largest_left->payload;
  node->payload = new_value;
  BinaryTreeNode** doomed_p = find_parent_pointer(largest_left);
  delete (doomed_p);
}

static size_t degree(BinaryTreeNode* node) {
  return (node->left != &NULL_NODE) + (node->right != &NULL_NODE);
}

static void delete (BinaryTreeNode** doomed) {
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

static void enumerate(BinaryTreeNode* node, item_handler payload_handler) {
  if (node == &NULL_NODE) return;

  enumerate(node->left, payload_handler);
  payload_handler(node->payload);
  enumerate(node->right, payload_handler);
}

static BinaryTreeNode* Min(BinaryTreeNode* root) {
  if (root->left == &NULL_NODE) return root;

  return Min(root->left);
}

static BinaryTreeNode* Max(BinaryTreeNode* root) {
  if (root->right == &NULL_NODE) return root;

  return Max(root->right);
}

static const BinaryTreeNode* parent_less_than(const BinaryTreeNode* root,
                                              const void* search_for,
                                              comparator comparator) {
  if (root == &NULL_NODE) return NULL;

  int result = comparator(search_for, root->payload);

  if (result > 0) return root;

  return parent_less_than(root->parent, search_for, comparator);
}

static const BinaryTreeNode* predecessor(BinaryTreeNode* root,
                                         const void* search_for,
                                         comparator comparator) {
  const BinaryTreeNode* node = traverse(root, search_for, comparator);
  if (node == NULL) {
    LIST_ERROR("Binary Tree", kkNotFound);
    return NULL;
  }

  if (node->left != &NULL_NODE) return Max(node->left);

  return parent_less_than(node->parent, search_for, comparator);
}

static const BinaryTreeNode* parent_greater_than(const BinaryTreeNode* root,
                                                 const void* search_for,
                                                 comparator comparator) {
  if (root == &NULL_NODE) return NULL;

  int result = comparator(search_for, root->payload);

  if (result < 0) return root;

  return parent_greater_than(root->parent, search_for, comparator);
}

static const BinaryTreeNode* Successor(BinaryTreeNode* root,
                                       const void* search_for,
                                       comparator comparator) {
  const BinaryTreeNode* node = traverse(root, search_for, comparator);
  if (node == NULL) {
    LIST_ERROR("Binary Tree", kkNotFound);
    return NULL;
  }

  if (node->right != &NULL_NODE) return Min(node->right);

  return parent_greater_than(node->parent, search_for, comparator);
}

static size_t node_size(const BinaryTreeNode* node) {
  return (node == 0) ? 0 : node->size;
}

static const BinaryTreeNode* Select(const BinaryTreeNode* root,
                                    const size_t index) {
  size_t left = node_size(root->left);

  if (left == index) return root;

  if (index < left) return Select(root->left, index);

  return Select(root->right, index - left - 1);
}

static size_t rank(const BinaryTreeNode* root, const void* payload,
                   comparator comparator, size_t offset) {
  if (root == &NULL_NODE) return RANK_ERROR;

  int result = comparator(payload, root->payload);

  size_t left = node_size(root->left);
  if (result == 0) return left + offset;

  if (result < 0) return rank(root->left, payload, comparator, offset);

  return rank(root->right, payload, comparator, offset + left + 1);
}

static void left_rotate(BinaryTree* self, BinaryTreeNode* pivot) {
  BinaryTreeNode* temp = pivot->right;
  pivot->right = temp->left;

  if (temp->left != &NULL_NODE) temp->left->parent = pivot;

  temp->parent = pivot->parent;

  if (pivot->parent == &NULL_NODE) {
    self->root = temp;
  } else if (pivot == pivot->parent->left) {
    pivot->parent->left = temp;
  } else {
    pivot->parent->right = temp;
  }

  temp->left = pivot;
  pivot->parent = temp;

  pivot->size = pivot->left->size + pivot->right->size + 1;
  pivot->parent->size = pivot->size + pivot->parent->right->size + 1;
}

static void right_rotate(BinaryTree* self, BinaryTreeNode* pivot) {
  BinaryTreeNode* temp = pivot->left;
  pivot->left = temp->right;

  if (temp->right != &NULL_NODE) temp->right->parent = pivot;

  temp->parent = pivot->parent;

  if (pivot->parent == &NULL_NODE) {
    self->root = temp;
  } else if (pivot == pivot->parent->right) {
    pivot->parent->right = temp;
  } else {
    pivot->parent->left = temp;
  }

  temp->right = pivot;
  pivot->parent = temp;

  pivot->size = pivot->left->size + pivot->right->size + 1;
  pivot->parent->size = pivot->size + pivot->parent->left->size + 1;
}

static int node_value(BinaryTreeNode* node) {
  if (node == &NULL_NODE) return -1;

  if (node == NULL) return -2;

  return *(int*)node->payload;
}

static char* color(COLOR color) {
  switch (color) {
    case RED:
      return "red";
    case BLACK:
      return "black";
    default:
      return "invalid";
  }
}

void BinaryTree_Print(BinaryTreeNode* node) {
  if (node == &NULL_NODE) return;

  if (node->left == &NULL_NODE && node->right == &NULL_NODE) return;

  printf("root=%d_%s, left=%d_%s, right=%d_%s\n", node_value(node),
         color(node->color), node_value(node->left), color(node->left->color),
         node_value(node->right), color(node->right->color));

  BinaryTree_Print(node->left);
  BinaryTree_Print(node->right);
}

static void balance(BinaryTree* tree, BinaryTreeNode* root) {
  while (root->parent->color == RED) {
    // parent->parent will never be null because this method should never be
    // called on the root node and the root node's parent is NULL_NODE.
    if (root->parent == root->parent->parent->left) {
      BinaryTreeNode* uncle = root->parent->parent->right;
      if (uncle->color == RED) {
        root->parent->color = BLACK;
        uncle->color = BLACK;
        root->parent->parent->color = RED;
        root = root->parent->parent;
      } else if (root == root->parent->right) {
        root = root->parent;
        left_rotate(tree, root);
      } else {
        root->parent->color = BLACK;
        root->parent->parent->color = RED;
        right_rotate(tree, root->parent->parent);
      }
    } else {
      BinaryTreeNode* uncle = root->parent->parent->left;
      if (uncle->color == RED) {
        root->parent->color = BLACK;
        uncle->color = BLACK;
        root->parent->parent->color = RED;
        root = root->parent->parent;
      } else if (root == root->parent->left) {
        root = root->parent;
        right_rotate(tree, root);
      } else {
        root->parent->color = BLACK;
        root->parent->parent->color = RED;
        left_rotate(tree, root->parent->parent);
      }
    }
  }

  tree->root->color = BLACK;
}

static void free_nodes(BinaryTreeNode* node, freer freer) {
  if (node == &NULL_NODE || node == NULL) return;

  free_nodes(node->left, freer);
  free_nodes(node->right, freer);

  tree_node_destroy(node, freer);
}

BinaryTree* BinaryTree_Create(comparator comparator) {
  if (comparator == NULL) {
    LIST_ERROR("Binary Tree", kkNullParameter);
    return NULL;
  }

  BinaryTree* tree = calloc(sizeof(BinaryTree), 1);
  if (tree == NULL) {
    LIST_ERROR("Binary Tree", kFailedMalloc);
    return NULL;
  }

  tree->comparator = comparator;
  tree->root = &NULL_NODE;
  return tree;
}

ListOpResult BinaryTree_Insert(BinaryTree* self, void* payload) {
  if (self == NULL || payload == NULL) return kkNullParameter;

  BinaryTreeNode* node = tree_node_create(payload);
  if (node == NULL) return kFailedMalloc;

  if (self->root == &NULL_NODE) {
    self->root = node;
  } else {
    insert_node(self->root, node, self->comparator);
  }

  self->n++;
  return kkSuccess;
}

void* BinaryTree_Delete(BinaryTree* self, void* payload) {
  if (self == NULL || payload == NULL) {
    LIST_ERROR("Binary Tree", kkNullParameter);
    return NULL;
  }

  if (self->n == 0) {
    LIST_ERROR("Binary Tree", kkEmptyList);
    return NULL;
  }

  BinaryTreeNode* doomed = traverse(self->root, payload, self->comparator);
  if (doomed == NULL) {
    LIST_ERROR("Binary Tree", kkNotFound);
    return NULL;
  }

  void* result = doomed->payload;

  if (doomed == self->root) {
    delete (&self->root);
  } else {
    delete (find_parent_pointer(doomed));
  }

  self->n--;
  return result;
}

void* BinaryTree_Search(const BinaryTree* self, const void* payload) {
  if (self == NULL || payload == NULL) {
    LIST_ERROR("Binary Tree", kkNullParameter);
    return NULL;
  }

  if (self->n == 0) {
    LIST_ERROR("Binary Tree", kkEmptyList);
    return NULL;
  }

  BinaryTreeNode* found = traverse(self->root, payload, self->comparator);
  if (found == NULL) {
    LIST_ERROR("Binary Tree", kkNotFound);
    return NULL;
  }

  return found->payload;
}

ListOpResult BinaryTree_Enumerate(const BinaryTree* self,
                                  item_handler payload_handler) {
  if (self == NULL || payload_handler == NULL) return kkNullParameter;

  enumerate(self->root, payload_handler);
  return kkSuccess;
}

void* BinaryTree_Min(const BinaryTree* self) {
  if (self == NULL) {
    LIST_ERROR("Binary Tree", kkNullParameter);
    return NULL;
  }

  if (self->n == 0) {
    LIST_ERROR("Binary Tree", kkEmptyList);
    return NULL;
  }

  BinaryTreeNode* node = Min(self->root);
  return node->payload;
}

void* BinaryTree_Max(const BinaryTree* self) {
  if (self == NULL) {
    LIST_ERROR("Binary Tree", kkNullParameter);
    return NULL;
  }

  if (self->n == 0) {
    LIST_ERROR("Binary Tree", kkEmptyList);
    return NULL;
  }

  BinaryTreeNode* node = Max(self->root);
  return node->payload;
}

void* BinaryTree_Predecessor(const BinaryTree* self, const void* payload) {
  if (self == NULL || payload == NULL) {
    LIST_ERROR("Binary Tree", kkNullParameter);
    return NULL;
  }

  if (self->n == 0) {
    LIST_ERROR("Binary Tree", kkEmptyList);
    return NULL;
  }

  const BinaryTreeNode* pred =
      predecessor(self->root, payload, self->comparator);
  if (pred == NULL) return NULL;

  return pred->payload;
}

void* BinaryTree_Successor(const BinaryTree* self, const void* payload) {
  if (self == NULL || payload == NULL) {
    LIST_ERROR("Binary Tree", kkNullParameter);
    return NULL;
  }

  if (self->n == 0) {
    LIST_ERROR("Binary Tree", kkEmptyList);
    return NULL;
  }

  const BinaryTreeNode* succ = Successor(self->root, payload, self->comparator);
  if (succ == NULL) return NULL;

  return succ->payload;
}

void* BinaryTree_Select(const BinaryTree* self, const size_t index) {
  if (self == NULL) {
    LIST_ERROR("Binary Tree", kkNullParameter);
    return NULL;
  }

  if (self->n == 0) {
    LIST_ERROR("Binary Tree", kkEmptyList);
    return NULL;
  }

  if (index >= self->n) {
    LIST_ERROR("Binary Tree", kkInvalidIndex);
    return NULL;
  }

  return Select(self->root, index)->payload;
}

size_t BinaryTree_Rank(const BinaryTree* self, const void* payload) {
  if (self == NULL || payload == NULL) {
    LIST_ERROR("Binary Tree", kkNullParameter);
    return RANK_ERROR;
  }

  if (self->n == 0) {
    LIST_ERROR("Binary Tree", kkEmptyList);
    return RANK_ERROR;
  }

  size_t result = rank(self->root, payload, self->comparator, 0);

  if (result == RANK_ERROR) LIST_ERROR("Binary Tree", kkNotFound);

  return result;
}

void BinaryTree_Destroy(BinaryTree* self, freer freer) {
  if (self == NULL) return;

  free_nodes(self->root, freer);
  free(self);
}

ListOpResult BinaryTree_RotateLeft(BinaryTree* self, const void* item) {
  if (self == NULL || item == NULL) return kkNullParameter;

  BinaryTreeNode* rotate_node = traverse(self->root, item, self->comparator);
  if (rotate_node == NULL) return kkNotFound;

  left_rotate(self, rotate_node);
  return kkSuccess;
}

ListOpResult BinaryTree_RotateRight(BinaryTree* self, const void* item) {
  if (self == NULL || item == NULL) return kkNullParameter;

  BinaryTreeNode* rotate_node = traverse(self->root, item, self->comparator);
  if (rotate_node == NULL) return kkNotFound;

  right_rotate(self, rotate_node);
  return kkSuccess;
}

ListOpResult RedBlackTree_Insert(BinaryTree* self, void* payload) {
  if (self == NULL || payload == NULL) return kkNullParameter;

  BinaryTreeNode* node = tree_node_create(payload);
  if (node == NULL) return kFailedMalloc;

  if (self->root == &NULL_NODE) {
    self->root = node;
  } else {
    insert_node(self->root, node, self->comparator);
    node->color = RED;
  }

  self->n++;
  balance(self, node);
  return kkSuccess;
}
