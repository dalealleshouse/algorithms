/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <inttypes.h>

#include "array.h"
#include "binary_tree.h"
#include "linked_list.h"
#include "sorted_array.h"

typedef enum {
  kInsert = 1,
  kSearch = 2,
  kEnumerate = 3,
  kMax = 4,
  kPredecessor = 5,
  kRank = 6
} Operation;

typedef enum {
  kArray = 1,
  kSortedArray = 2,
  kLinkedList = 3,
  kLinkedListPoorLocality = 4,
  kBinaryTree = 5,
  kBinaryTreeUnbalanced = 6,
  kRedBlackTree = 7
} Structure;

double ListDataStructures_OperationTime(Operation, Structure, size_t);
