// Copyright 2020 Hideous Humpback Freak https://hideoushumpbackfreak.com/
#include "binary_tree.h"

#include <stdint.h>
#include <stdlib.h>

BinaryTreeNode kNullNode = {NULL, NULL, NULL, NULL, 0, kBlack};

static void delete (BinaryTreeNode**);
static ResultCode Max(BinaryTreeNode*, BinaryTreeNode**);

static void tree_node_destroy(BinaryTreeNode* node, freer freer) {
  if (node == &kNullNode) return;

  if (freer != NULL) freer(node->payload);

  free(node);
}

static BinaryTreeNode* tree_node_create(void* payload) {
  BinaryTreeNode* node = calloc(sizeof(BinaryTreeNode), 1);
  if (node == NULL) return NULL;

  node->parent = &kNullNode;
  node->left = &kNullNode;
  node->right = &kNullNode;
  node->payload = payload;
  node->size = 1;
  node->color = kBlack;

  return node;
}

static BinaryTreeNode* traverse(BinaryTreeNode* node, const void* search_for,
                                comparator comparator) {
  if (node == &kNullNode) return NULL;

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
    if (root->left == &kNullNode) {
      root->left = new;
      new->parent = root;
    } else {
      insert_node(root->left, new, comparator);
    }
    return;
  }

  if (root->right == &kNullNode) {
    root->right = new;
    new->parent = root;
  } else {
    insert_node(root->right, new, comparator);
  }
}

static void decrement_size(BinaryTreeNode* node) {
  if (node == &kNullNode) return;

  decrement_size(node->parent);
  node->size--;
}

static void delete_leaf(BinaryTreeNode** doomed) {
  BinaryTreeNode* node = *doomed;

  decrement_size(node);

  tree_node_destroy(node, NULL);
  *doomed = &kNullNode;
}

static void delete_degree_one(BinaryTreeNode** doomed) {
  BinaryTreeNode* node = *doomed;

  decrement_size(node);
  BinaryTreeNode* child = (node->left != &kNullNode) ? node->left : node->right;
  child->parent = node->parent;
  tree_node_destroy(node, NULL);
  *doomed = child;
}

static BinaryTreeNode** find_parent_pointer(BinaryTreeNode* node) {
  BinaryTreeNode* parent = node->parent;

  if (parent->left == node) return &parent->left;

  return &parent->right;
}

static ResultCode delete_degree_two(BinaryTreeNode** doomed) {
  BinaryTreeNode* node = *doomed;

  BinaryTreeNode* largest_left = NULL;
  ResultCode result_code = Max(node->left, &largest_left);
  if (result_code != kSuccess) return result_code;

  void* new_value = largest_left->payload;
  node->payload = new_value;
  BinaryTreeNode** doomed_p = find_parent_pointer(largest_left);
  delete (doomed_p);
  return kSuccess;
}

static size_t degree(BinaryTreeNode* node) {
  return (node->left != &kNullNode) + (node->right != &kNullNode);
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
  if (node == &kNullNode) return;

  enumerate(node->left, payload_handler);
  payload_handler(node->payload);
  enumerate(node->right, payload_handler);
}

static ResultCode Min(BinaryTreeNode* root, BinaryTreeNode** result) {
  if (root->left == &kNullNode) {
    *result = root;
    return kSuccess;
  }

  return Min(root->left, result);
}

static ResultCode Max(BinaryTreeNode* root, BinaryTreeNode** result) {
  if (root->right == &kNullNode) {
    *result = root;
    return kSuccess;
  }

  return Max(root->right, result);
}

static ResultCode parent_less_than(BinaryTreeNode* root, const void* search_for,
                                   comparator comparator,
                                   BinaryTreeNode** result) {
  if (root == &kNullNode) return kArgumentOutOfRange;

  int comp_result = comparator(search_for, root->payload);

  if (comp_result > 0) {
    *result = root;
    return kSuccess;
  }

  return parent_less_than(root->parent, search_for, comparator, result);
}

static ResultCode predecessor(BinaryTreeNode* root, const void* search_for,
                              comparator comparator, BinaryTreeNode** result) {
  const BinaryTreeNode* node = traverse(root, search_for, comparator);
  if (node == NULL) return kNotFound;

  if (node->left != &kNullNode) return Max(node->left, result);

  return parent_less_than(node->parent, search_for, comparator, result);
}

static ResultCode parent_greater_than(BinaryTreeNode* root,
                                      const void* search_for,
                                      comparator comparator,
                                      BinaryTreeNode** result) {
  if (root == &kNullNode) return kArgumentOutOfRange;

  int comp_result = comparator(search_for, root->payload);

  if (comp_result < 0) {
    *result = root;
    return kSuccess;
  }

  return parent_greater_than(root->parent, search_for, comparator, result);
}

static ResultCode Successor(BinaryTreeNode* root, const void* search_for,
                            comparator comparator, BinaryTreeNode** result) {
  const BinaryTreeNode* node = traverse(root, search_for, comparator);
  if (node == NULL) return kNotFound;
  if (node->right != &kNullNode) return Min(node->right, result);

  return parent_greater_than(node->parent, search_for, comparator, result);
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

static ResultCode rank(const BinaryTreeNode* root, const void* payload,
                       comparator comparator, size_t offset, size_t* result) {
  if (root == &kNullNode) return kNotFound;

  int comp_result = comparator(payload, root->payload);

  size_t left = node_size(root->left);
  if (comp_result == 0) {
    *result = left + offset;
    return kSuccess;
  }

  if (comp_result < 0) {
    return rank(root->left, payload, comparator, offset, result);
  }

  return rank(root->right, payload, comparator, offset + left + 1, result);
}

static void left_rotate(BinaryTree* self, BinaryTreeNode* pivot) {
  BinaryTreeNode* temp = pivot->right;
  pivot->right = temp->left;

  if (temp->left != &kNullNode) temp->left->parent = pivot;

  temp->parent = pivot->parent;

  if (pivot->parent == &kNullNode) {
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

  if (temp->right != &kNullNode) temp->right->parent = pivot;

  temp->parent = pivot->parent;

  if (pivot->parent == &kNullNode) {
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
  if (node == &kNullNode) return -1;

  if (node == NULL) return -2;

  return *(int*)node->payload;
}

static char* color(Color color) {
  switch (color) {
    case kRed:
      return "red";
    case kBlack:
      return "black";
    default:
      return "invalid";
  }
}

void BinaryTree_Print(BinaryTreeNode* node) {
  if (node == &kNullNode) return;

  if (node->left == &kNullNode && node->right == &kNullNode) return;

  printf("root=%d_%s, left=%d_%s, right=%d_%s\n", node_value(node),
         color(node->color), node_value(node->left), color(node->left->color),
         node_value(node->right), color(node->right->color));

  BinaryTree_Print(node->left);
  BinaryTree_Print(node->right);
}

static void balance(BinaryTree* tree, BinaryTreeNode* root) {
  while (root->parent->color == kRed) {
    // parent->parent will never be null because this method should never be
    // called on the root node and the root node's parent is kNullNode.
    if (root->parent == root->parent->parent->left) {
      BinaryTreeNode* uncle = root->parent->parent->right;
      if (uncle->color == kRed) {
        root->parent->color = kBlack;
        uncle->color = kBlack;
        root->parent->parent->color = kRed;
        root = root->parent->parent;
      } else if (root == root->parent->right) {
        root = root->parent;
        left_rotate(tree, root);
      } else {
        root->parent->color = kBlack;
        root->parent->parent->color = kRed;
        right_rotate(tree, root->parent->parent);
      }
    } else {
      BinaryTreeNode* uncle = root->parent->parent->left;
      if (uncle->color == kRed) {
        root->parent->color = kBlack;
        uncle->color = kBlack;
        root->parent->parent->color = kRed;
        root = root->parent->parent;
      } else if (root == root->parent->left) {
        root = root->parent;
        right_rotate(tree, root);
      } else {
        root->parent->color = kBlack;
        root->parent->parent->color = kRed;
        left_rotate(tree, root->parent->parent);
      }
    }
  }

  tree->root->color = kBlack;
}

static void free_nodes(BinaryTreeNode* node, freer freer) {
  if (node == &kNullNode || node == NULL) return;

  free_nodes(node->left, freer);
  free_nodes(node->right, freer);

  tree_node_destroy(node, freer);
}

ResultCode BinaryTree_Create(comparator comparator, BinaryTree** self) {
  if (comparator == NULL) return kNullParameter;
  if (*self != NULL) return kOutputPointerIsNotNull;

  BinaryTree* tree = calloc(sizeof(BinaryTree), 1);
  if (tree == NULL) return kFailedMemoryAllocation;

  tree->comparator = comparator;
  tree->root = &kNullNode;
  *self = tree;
  return kSuccess;
}

ResultCode BinaryTree_Insert(BinaryTree* self, void* payload) {
  if (self == NULL || payload == NULL) return kNullParameter;

  BinaryTreeNode* node = tree_node_create(payload);
  if (node == NULL) return kFailedMemoryAllocation;

  if (self->root == &kNullNode) {
    self->root = node;
  } else {
    insert_node(self->root, node, self->comparator);
  }

  self->n++;
  return kSuccess;
}

ResultCode BinaryTree_Delete(BinaryTree* self, void* payload, void** result) {
  if (self == NULL || payload == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (self->n == 0) return kEmpty;

  BinaryTreeNode* doomed = traverse(self->root, payload, self->comparator);
  if (doomed == NULL) return kNotFound;

  void* temp = doomed->payload;

  if (doomed == self->root) {
    delete (&self->root);
  } else {
    delete (find_parent_pointer(doomed));
  }

  self->n--;
  *result = temp;
  return kSuccess;
}

ResultCode BinaryTree_Search(const BinaryTree* self, const void* payload,
                             void** result) {
  if (self == NULL || payload == NULL) return kNullParameter;
  if (self->n == 0) return kEmpty;

  BinaryTreeNode* found = traverse(self->root, payload, self->comparator);
  if (found == NULL) return kNotFound;

  *result = found->payload;
  return kSuccess;
}

ResultCode BinaryTree_Enumerate(const BinaryTree* self,
                                item_handler payload_handler) {
  if (self == NULL || payload_handler == NULL) return kNullParameter;

  enumerate(self->root, payload_handler);
  return kSuccess;
}

ResultCode BinaryTree_Min(const BinaryTree* self, void** result) {
  if (self == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (self->n == 0) return kEmpty;

  BinaryTreeNode* node = NULL;
  ResultCode result_code = Min(self->root, &node);
  if (result_code != kSuccess) return result_code;

  *result = node->payload;
  return kSuccess;
}

ResultCode BinaryTree_Max(const BinaryTree* self, void** result) {
  if (self == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (self->n == 0) return kEmpty;

  BinaryTreeNode* node = NULL;
  ResultCode result_code = Max(self->root, &node);
  if (result_code != kSuccess) return result_code;

  *result = node->payload;
  return kSuccess;
}

ResultCode BinaryTree_Predecessor(const BinaryTree* self, const void* payload,
                                  void** result) {
  if (self == NULL || payload == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (self->n == 0) return kEmpty;

  BinaryTreeNode* pred = NULL;
  ResultCode result_code =
      predecessor(self->root, payload, self->comparator, &pred);
  if (result_code != kSuccess) return result_code;

  *result = pred->payload;
  return kSuccess;
}

ResultCode BinaryTree_Successor(const BinaryTree* self, const void* payload,
                                void** result) {
  if (self == NULL || payload == NULL || result == NULL) return kNullParameter;
  if (self->n == 0) return kEmpty;

  BinaryTreeNode* succ = NULL;
  ResultCode result_code =
      Successor(self->root, payload, self->comparator, &succ);
  if (result_code != kSuccess) return result_code;

  *result = succ->payload;
  return kSuccess;
}

ResultCode BinaryTree_Select(const BinaryTree* self, const size_t index,
                             void** result) {
  if (self == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (self->n == 0) return kEmpty;
  if (index >= self->n) return kInvalidIndex;

  *result = Select(self->root, index)->payload;
  return kSuccess;
}

ResultCode BinaryTree_Rank(const BinaryTree* self, const void* payload,
                           size_t* result) {
  if (self == NULL || payload == NULL || result == NULL) return kNullParameter;
  if (self->n == 0) return kEmpty;

  if (self->root == &kNullNode) return kNotFound;

  return rank(self->root, payload, self->comparator, 0, result);
}

void BinaryTree_Destroy(BinaryTree* self, freer freer) {
  if (self == NULL) return;

  free_nodes(self->root, freer);
  free(self);
}

ResultCode BinaryTree_RotateLeft(BinaryTree* self, const void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  BinaryTreeNode* rotate_node = traverse(self->root, item, self->comparator);
  if (rotate_node == NULL) return kNotFound;

  left_rotate(self, rotate_node);
  return kSuccess;
}

ResultCode BinaryTree_RotateRight(BinaryTree* self, const void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  BinaryTreeNode* rotate_node = traverse(self->root, item, self->comparator);
  if (rotate_node == NULL) return kNotFound;

  right_rotate(self, rotate_node);
  return kSuccess;
}

ResultCode RedBlackTree_Insert(BinaryTree* self, void* payload) {
  if (self == NULL || payload == NULL) return kNullParameter;

  BinaryTreeNode* node = tree_node_create(payload);
  if (node == NULL) return kFailedMemoryAllocation;

  if (self->root == &kNullNode) {
    self->root = node;
  } else {
    insert_node(self->root, node, self->comparator);
    node->color = kRed;
  }

  self->n++;
  balance(self, node);
  return kSuccess;
}
