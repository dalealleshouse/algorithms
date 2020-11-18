/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "binary_tree.h"

#include <stdint.h>
#include <stdlib.h>

BinaryTreeNode kNullNode = {NULL, NULL, NULL, NULL, 0, kBlack};

typedef struct DeletedContext {
  struct BinaryTreeNode* replacement;
  Color original_color;
} DeletedContext;

static void Delete(BinaryTreeNode**);
static void RedBlackDelete(BinaryTreeNode**, DeletedContext* context);
static ResultCode Max(BinaryTreeNode*, BinaryTreeNode**);
static ResultCode Min(BinaryTreeNode*, BinaryTreeNode**);

static void TreeNodeDestroy(BinaryTreeNode* node, freer freer) {
  if (node == &kNullNode) return;

  if (freer != NULL) freer(node->payload);

  free(node);
}

static BinaryTreeNode* TreeNodeCreate(void* payload) {
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

static BinaryTreeNode* Traverse(BinaryTreeNode* node, const void* search_for,
                                sort_strategy comparator) {
  if (node == &kNullNode) return NULL;

  int result = comparator(search_for, node->payload);

  if (result == 0) return node;

  if (result < 0) return Traverse(node->left, search_for, comparator);

  return Traverse(node->right, search_for, comparator);
}

static void InsertNode(BinaryTreeNode* root, BinaryTreeNode* new,
                       sort_strategy comparator) {
  root->size++;
  int result = comparator(new->payload, root->payload);

  if (result <= 0) {
    if (root->left == &kNullNode) {
      root->left = new;
      new->parent = root;
    } else {
      InsertNode(root->left, new, comparator);
    }
    return;
  }

  if (root->right == &kNullNode) {
    root->right = new;
    new->parent = root;
  } else {
    InsertNode(root->right, new, comparator);
  }
}

static void DecrementSize(BinaryTreeNode* node) {
  if (node == &kNullNode) return;

  DecrementSize(node->parent);
  node->size--;
}

static void DeleteDegreeOne(BinaryTreeNode** doomed, DeletedContext* context) {
  BinaryTreeNode* node = *doomed;

  DecrementSize(node);
  BinaryTreeNode* child = (node->left != &kNullNode) ? node->left : node->right;
  context->replacement = child;
  child->parent = node->parent;
  TreeNodeDestroy(node, NULL);
  *doomed = child;
}

static BinaryTreeNode** FindParentPointer(BinaryTreeNode* node) {
  BinaryTreeNode* parent = node->parent;
  return (parent->left == node) ? &parent->left : &parent->right;
}

static ResultCode DeleteDegreeTwo(BinaryTreeNode** doomed,
                                  DeletedContext* context) {
  BinaryTreeNode* node = *doomed;

  BinaryTreeNode* biggest_left = NULL;
  ResultCode result_code = Max(node->left, &biggest_left);
  if (result_code != kSuccess) return result_code;

  context->original_color = biggest_left->color;

  node->payload = biggest_left->payload;
  BinaryTreeNode** doomed_p = FindParentPointer(biggest_left);
  RedBlackDelete(doomed_p, context);
  return kSuccess;
}

static size_t Degree(BinaryTreeNode* node) {
  return (node->left != &kNullNode) + (node->right != &kNullNode);
}

static void RedBlackDelete(BinaryTreeNode** doomed, DeletedContext* context) {
  BinaryTreeNode* node = *doomed;
  context->original_color = (*doomed)->color;

  size_t deg = Degree(node);

  switch (deg) {
    case 0:
    case 1:
      DeleteDegreeOne(doomed, context);
      break;
    case 2:
      DeleteDegreeTwo(doomed, context);
      break;
  }
}

static void Delete(BinaryTreeNode** doomed) {
  DeletedContext context;
  RedBlackDelete(doomed, &context);
}

static void Enumerate(BinaryTreeNode* node, item_handler payload_handler) {
  if (node == &kNullNode) return;

  Enumerate(node->left, payload_handler);
  payload_handler(node->payload);
  Enumerate(node->right, payload_handler);
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

static ResultCode ParentLessThan(BinaryTreeNode* root, const void* search_for,
                                 sort_strategy comparator,
                                 BinaryTreeNode** result) {
  if (root == &kNullNode) return kArgumentOutOfRange;

  int comp_result = comparator(search_for, root->payload);

  if (comp_result > 0) {
    *result = root;
    return kSuccess;
  }

  return ParentLessThan(root->parent, search_for, comparator, result);
}

static ResultCode Predecessor(BinaryTreeNode* root, const void* search_for,
                              sort_strategy comparator,
                              BinaryTreeNode** result) {
  const BinaryTreeNode* node = Traverse(root, search_for, comparator);
  if (node == NULL) return kNotFound;

  if (node->left != &kNullNode) return Max(node->left, result);

  return ParentLessThan(node->parent, search_for, comparator, result);
}

static ResultCode ParentGreaterThan(BinaryTreeNode* root,
                                    const void* search_for,
                                    sort_strategy comparator,
                                    BinaryTreeNode** result) {
  if (root == &kNullNode) return kArgumentOutOfRange;

  int comp_result = comparator(search_for, root->payload);

  if (comp_result < 0) {
    *result = root;
    return kSuccess;
  }

  return ParentGreaterThan(root->parent, search_for, comparator, result);
}

static ResultCode Successor(BinaryTreeNode* root, const void* search_for,
                            sort_strategy comparator, BinaryTreeNode** result) {
  const BinaryTreeNode* node = Traverse(root, search_for, comparator);
  if (node == NULL) return kNotFound;
  if (node->right != &kNullNode) return Min(node->right, result);

  return ParentGreaterThan(node->parent, search_for, comparator, result);
}

static size_t NodeSize(const BinaryTreeNode* node) {
  return (node == 0) ? 0 : node->size;
}

static const BinaryTreeNode* Select(const BinaryTreeNode* root,
                                    const size_t index) {
  size_t left = NodeSize(root->left);

  if (left == index) return root;

  if (index < left) return Select(root->left, index);

  return Select(root->right, index - left - 1);
}

static ResultCode rank(const BinaryTreeNode* root, const void* payload,
                       sort_strategy comparator, size_t offset,
                       size_t* result) {
  if (root == &kNullNode) return kNotFound;

  int comp_result = comparator(payload, root->payload);

  size_t left = NodeSize(root->left);
  if (comp_result == 0) {
    *result = left + offset;
    return kSuccess;
  }

  if (comp_result < 0) {
    return rank(root->left, payload, comparator, offset, result);
  }

  return rank(root->right, payload, comparator, offset + left + 1, result);
}

static void LeftRotate(BinaryTree* self, BinaryTreeNode* pivot) {
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

static void RightRotate(BinaryTree* self, BinaryTreeNode* pivot) {
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

static int NodeValue(BinaryTreeNode* node) {
  if (node == &kNullNode) return -1;

  if (node == NULL) return -2;

  return *(int*)node->payload;
}

static char* color_as_string(Color color) {
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

  printf("root=%d_%s, left=%d_%s, right=%d_%s\n", NodeValue(node),
         color_as_string(node->color), NodeValue(node->left),
         color_as_string(node->left->color), NodeValue(node->right),
         color_as_string(node->right->color));

  BinaryTree_Print(node->left);
  BinaryTree_Print(node->right);
}

static void Balance(BinaryTree* tree, BinaryTreeNode* root) {
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
        LeftRotate(tree, root);
      } else {
        root->parent->color = kBlack;
        root->parent->parent->color = kRed;
        RightRotate(tree, root->parent->parent);
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
        RightRotate(tree, root);
      } else {
        root->parent->color = kBlack;
        root->parent->parent->color = kRed;
        LeftRotate(tree, root->parent->parent);
      }
    }
  }

  tree->root->color = kBlack;
}

static ResultCode BalanceAfterDelete(BinaryTree* tree, BinaryTreeNode* node) {
  BinaryTreeNode* s = NULL;
  if (node == NULL) return kNullParameter;

  while (node != tree->root && node->color == kBlack) {
    if (node == node->parent->left) {
      s = node->parent->right;
      if (s->color == kRed) {
        // case 3.1
        s->color = kBlack;
        node->parent->color = kRed;
        LeftRotate(tree, node->parent);
        s = node->parent->right;
      }

      if (s->left->color == kBlack && s->right->color == kBlack) {
        // case 3.2
        s->color = kRed;
        node = node->parent;
      } else {
        if (s->right->color == kBlack) {
          // case 3.3
          s->left->color = kBlack;
          s->color = kRed;
          RightRotate(tree, s);
          s = node->parent->right;
        }

        // case 3.4
        s->color = node->parent->color;
        node->parent->color = kBlack;
        s->right->color = kBlack;
        LeftRotate(tree, node->parent);
        node = tree->root;
      }
    } else {
      s = node->parent->left;
      if (s->color == kRed) {
        // case 3.1
        s->color = kBlack;
        node->parent->color = kRed;
        RightRotate(tree, node->parent);
        s = node->parent->left;
      }

      if (s->left->color == kBlack && s->right->color == kBlack) {
        // case 3.2
        s->color = kRed;
        node = node->parent;
      } else {
        if (s->left->color == kBlack) {
          // case 3.3
          s->right->color = kBlack;
          s->color = kRed;
          LeftRotate(tree, s);
          s = node->parent->left;
        }

        // case 3.4
        s->color = node->parent->color;
        node->parent->color = kBlack;
        s->left->color = kBlack;
        RightRotate(tree, node->parent);
        node = tree->root;
      }
    }
  }

  node->color = kBlack;
  kNullNode.parent = NULL;
  return kSuccess;
}

static void FreeNodes(BinaryTreeNode* node, freer freer) {
  if (node == &kNullNode || node == NULL) return;

  FreeNodes(node->left, freer);
  FreeNodes(node->right, freer);

  TreeNodeDestroy(node, freer);
}

ResultCode BinaryTree_Create(sort_strategy comparator, BinaryTree** self) {
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

  BinaryTreeNode* node = TreeNodeCreate(payload);
  if (node == NULL) return kFailedMemoryAllocation;

  if (self->root == &kNullNode) {
    self->root = node;
  } else {
    InsertNode(self->root, node, self->comparator);
  }

  self->n++;
  return kSuccess;
}

ResultCode BinaryTree_Delete(BinaryTree* self, void* payload, void** result) {
  if (self == NULL || payload == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (self->n == 0) return kEmpty;

  BinaryTreeNode* doomed = Traverse(self->root, payload, self->comparator);
  if (doomed == NULL) return kNotFound;

  void* temp = doomed->payload;

  if (doomed == self->root) {
    Delete(&self->root);
  } else {
    Delete(FindParentPointer(doomed));
  }

  self->n--;
  *result = temp;
  return kSuccess;
}

ResultCode BinaryTree_Search(const BinaryTree* self, const void* payload,
                             void** result) {
  if (self == NULL || payload == NULL) return kNullParameter;
  if (self->n == 0) return kEmpty;

  BinaryTreeNode* found = Traverse(self->root, payload, self->comparator);
  if (found == NULL) return kNotFound;

  *result = found->payload;
  return kSuccess;
}

ResultCode BinaryTree_Enumerate(const BinaryTree* self,
                                item_handler payload_handler) {
  if (self == NULL || payload_handler == NULL) return kNullParameter;

  Enumerate(self->root, payload_handler);
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
      Predecessor(self->root, payload, self->comparator, &pred);
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

  FreeNodes(self->root, freer);
  free(self);
}

ResultCode BinaryTree_RotateLeft(BinaryTree* self, const void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  BinaryTreeNode* rotate_node = Traverse(self->root, item, self->comparator);
  if (rotate_node == NULL) return kNotFound;

  LeftRotate(self, rotate_node);
  return kSuccess;
}

ResultCode BinaryTree_RotateRight(BinaryTree* self, const void* item) {
  if (self == NULL || item == NULL) return kNullParameter;

  BinaryTreeNode* rotate_node = Traverse(self->root, item, self->comparator);
  if (rotate_node == NULL) return kNotFound;

  RightRotate(self, rotate_node);
  return kSuccess;
}

ResultCode RedBlackTree_Insert(BinaryTree* self, void* payload) {
  if (self == NULL || payload == NULL) return kNullParameter;

  BinaryTreeNode* node = TreeNodeCreate(payload);
  if (node == NULL) return kFailedMemoryAllocation;

  if (self->root == &kNullNode) {
    self->root = node;
  } else {
    InsertNode(self->root, node, self->comparator);
    node->color = kRed;
  }

  self->n++;
  Balance(self, node);
  return kSuccess;
}

ResultCode RedBlackTree_Delete(BinaryTree* self, void* payload, void** result) {
  if (self == NULL || payload == NULL) return kNullParameter;
  if (*result != NULL) return kOutputPointerIsNotNull;
  if (self->n == 0) return kEmpty;

  BinaryTreeNode* doomed = Traverse(self->root, payload, self->comparator);
  if (doomed == NULL) return kNotFound;

  void* temp = doomed->payload;
  DeletedContext context = {NULL, kInvalid};

  if (doomed == self->root) {
    RedBlackDelete(&self->root, &context);
  } else {
    RedBlackDelete(FindParentPointer(doomed), &context);
  }

  self->n--;
  if (context.original_color == kBlack) {
    ResultCode balance_result = BalanceAfterDelete(self, context.replacement);
    if (balance_result != kSuccess) return balance_result;
  }

  *result = temp;
  return kSuccess;
}
