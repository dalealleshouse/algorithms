/*******************************************************************************
 * This is a rather anomalous implementation because it's rarely the case that
 * anyone would want to insert/delete an item into a linked list at a particular
 * index.  However, it meets the needs of this project because it easily enables
 * the creation of a list with poor spatial locality. This enables a
 * demonstration of the performance implications of sequentially accessing items
 * that are not contiguous in memory.
 ******************************************************************************/
#pragma once

#include <stddef.h>

#include "ListOperations.h"

typedef void (*freer)(void* x);

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

LinkedList* LinkedList_Create(freer, comparator);
ListOpResult LinkedList_InsertAt(LinkedList*, void*, const size_t);
ListOpResult LinkedList_DeleteAt(LinkedList*, const size_t);
void* LinkedList_Search(const LinkedList*, const void*);
ListOpResult LinkedList_Enumerate(const LinkedList*, item_handler);
void LinkedList_Destroy(LinkedList*);
