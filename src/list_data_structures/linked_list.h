// Copyright 2020 Hideous Humpback Freak https://hideoushumpbackfreak.com/
#pragma once

#include <stddef.h>

#include "../utils/comparators.h"
#include "../utils/result_code.h"

typedef void (*item_handler)(void* x);

typedef struct LinkedListItem {
  void* payload;
  struct LinkedListItem* next;
  struct LinkedListItem* prev;
} LinkedListItem;

typedef struct {
  size_t size;
  freer freer;
  comparator comparator;
  LinkedListItem* head;
  LinkedListItem* tail;
} LinkedList;

ResultCode LinkedList_Create(freer, comparator, LinkedList**);
ResultCode LinkedList_InsertAt(LinkedList*, void*, const size_t);
ResultCode LinkedList_DeleteAt(LinkedList*, const size_t);
ResultCode LinkedList_Delete(LinkedList*, void*);
ResultCode LinkedList_Search(const LinkedList*, const void*, void**);
ResultCode LinkedList_Enumerate(const LinkedList*, item_handler);
void LinkedList_Destroy(LinkedList*);
