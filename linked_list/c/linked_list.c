#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "linked_list.h"

/*
 * We are going to assume if malloc fails, we can't recover
 */
void* Malloc(size_t size)
{
    void* p = malloc(size);

    if (p == NULL) {
        fprintf(stderr, "malloc failed");

        exit(EXIT_FAILURE);
    }

    return p;
}

int list_init(list* list)
{
    if (list == NULL)
        return -1;

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return 0;
}

static list_item* list_item_init(void* payload, const freer free_me)
{
    list_item* item = Malloc(sizeof(list_item));

    item->next = NULL;
    item->prev = NULL;
    item->free_me = free_me;
    item->payload = payload;

    return item;
}

static void free_list_item(list_item* doomed)
{
    if (doomed->free_me != NULL)
        doomed->free_me(doomed->payload);

    free(doomed);
}

int list_free(list* list)
{
    if (list == NULL)
        return -1;

    list_item* curr = list->head;

    while (curr != NULL) {
        list_item* doomed = curr;
        curr = curr->next;

        free_list_item(doomed);
    };

    return list_init(list);
}

/*
 * ASSUMPTION: No NULL values
 */
static void insert_at_head(list* list, list_item* item)
{
    if (list->head == NULL) {
        list->head = item;
        list->tail = item;
        return;
    }

    item->prev = NULL;
    list->head->prev = item;

    item->next = list->head;
    list->head = item;
}

/*
 * ASSUMPTIONS:
 *  - No NULL input values
 *  - head and tail are not NULL
 */
static void insert_at_tail(list* list, list_item* item)
{
    item->prev = list->tail;
    list->tail->next = item;
    list->tail = item;
}

/*
 * ASSUMPTIONS:
 *  - No NULL input values
 *  - head and tail are not NULL
 *  - index is valid
 */
static void insert_mid(list* list, list_item* item, size_t index)
{
    list_item* curr = list->head;
    for (size_t i = 1; i < index; i++)
        curr = curr->next;

    item->prev = curr;
    item->next = curr->next;

    curr->next->prev = item;
    curr->next = item;
}

int list_insert_at(list* list, void* payload, const size_t index)
{
    return list_insert_at_free(list, payload, NULL, index);
}

int list_insert_at_free(
    list* list, void* payload, const freer free_me, const size_t index)
{
    if (list == NULL || index > list->size)
        return -1;

    list_item* item = list_item_init(payload, free_me);

    if (index == 0)
        insert_at_head(list, item);
    else if (index == list->size)
        insert_at_tail(list, item);
    else
        insert_mid(list, item, index);

    list->size++;

    return 0;
}

/*
 * ASSUMPTION: No NULL values
 */
static void delete_at_head(list* list)
{
    list_item* head = list->head;

    if (head == list->tail) {
        list->head = NULL;
        list->tail = NULL;
    } else
        list->head = head->next;

    if (list->head != NULL)
        list->head->prev = NULL;

    free_list_item(head);
}

/*
 * ASSUMPTIONS:
 *  - No NULL values
 *  - Will never been called when there is only 1 item in the list
 */
static void delete_at_tail(list* list)
{
    list_item* tail = list->tail;

    tail->prev->next = NULL;
    list->tail = tail->prev;

    free_list_item(tail);
}

/*
 * ASSUMPTIONS:
 *  - No NULL values
 *  - Will never been called when there is only 1 item in the list
 */
static void delete_at_mid(list* list, size_t index)
{
    list_item* item = list->head;

    for (size_t i = 0; i < index; i++)
        item = item->next;

    list_item* prev = item->prev;
    list_item* next = item->next;
    prev->next = next;
    next->prev = prev;

    free_list_item(item);
}

int list_delete_at(list* list, const size_t index)
{
    if (list == NULL || index >= list->size)
        return -1;

    if (index == 0)
        delete_at_head(list);
    else if (index == list->size - 1)
        delete_at_tail(list);
    else
        delete_at_mid(list, index);

    list->size--;
    return 0;
}

void* list_get_at(list* list, const size_t index)
{
    if (list == NULL || index >= list->size)
        return NULL;

    if (index == 0)
        return list->head->payload;

    if (index == list->size - 1)
        return list->tail->payload;

    list_item* item;

    // Count forward if the item is in the first half
    if (index < list->size / 2) {
        item = list->head;
        for (size_t i = 0; i < index; i++)
            item = item->next;
    } else {
        // Count backward if the item is in the second half
        item = list->tail;
        size_t rev_index = list->size - index;
        for (size_t i = 1; i < rev_index; i++)
            item = item->prev;
    }

    return item->payload;
}

void* array_insert(void* arr, const size_t n, const size_t size)
{
    if (arr == NULL)
        return NULL;

    void* new_arr = Malloc((n + 1) * size);
    memcpy((char*)new_arr + size, arr, n * size);
    return new_arr;
}
