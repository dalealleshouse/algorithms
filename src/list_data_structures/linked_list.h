/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>

#include "comparators.h"
#include "result_code.h"

typedef void (*item_handler)(void* x);

typedef struct LinkedListItem {
  void* payload;
  struct LinkedListItem* next;
  struct LinkedListItem* prev;
} LinkedListItem;

typedef struct {
  size_t size;
  freer freer;
  sort_strategy comparator;
  LinkedListItem* head;
  LinkedListItem* tail;
} LinkedList;

ResultCode LinkedList_Create(freer, sort_strategy, LinkedList**);
ResultCode LinkedList_InsertAt(LinkedList*, void*, const size_t);
ResultCode LinkedList_DeleteAt(LinkedList*, const size_t);
ResultCode LinkedList_Delete(LinkedList*, void*);
ResultCode LinkedList_Search(const LinkedList*, const void*, void**);
ResultCode LinkedList_Enumerate(const LinkedList*, item_handler);
ResultCode LinkedList_Max(const LinkedList*, void**);
ResultCode LinkedList_Predecessor(const LinkedList*, const void*, void**);
ResultCode LinkedList_Rank(const LinkedList*, const void*, size_t*);
void LinkedList_Destroy(LinkedList*);
