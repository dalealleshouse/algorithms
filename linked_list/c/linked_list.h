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

typedef void (*freer)(void* x);

typedef struct list_item {
    void* payload;
    struct list_item* next;
    struct list_item* prev;
    freer free_me;
} list_item;

typedef struct {
    size_t size;
    list_item* head;
    list_item* tail;
} list;

/*
 * Initializes a list with default values
 *
 * returns: 0 on success, less than 0 on failure
 */
int list_init(list* list);

/*
 * Frees all list items and calls the freer function on payloads
 *
 * returns: 0 on success, less than 0 on failure
 */
int list_free(list* list);

/*
 * Wraps <payload> in a <list_item> and inserts it in the <index> position of
 * <list>. The caller is responsible for freeing <payload>
 *
 * returns: 0 on success, less than 0 on failure
 */
int list_insert_at(list* list, void* payload, const size_t index);

/*
 * Wraps <payload> in a <list_item> and inserts it in the <index> position of
 * <list>. When the item is disposed, <free_me> is called on <payload>.
 *
 * returns: 0 on success, less than 0 on failure
 */
int list_insert_at_free(
    list* list, void* payload, const freer free_me, const size_t index);

/*
 * Removes an item from the list, calls the <freer> (if supplied at insert), and
 * frees the list item.
 *
 * returns: 0 on success, less than 0 on failure
 */
int list_delete_at(list* list, const size_t index);

/*
 * Get the payload stored in the <index>th position.
 *
 * returns: payload on success, null on failure
 */
void* list_get_at(list* list, const size_t index);

/*
 * Reallocates a new larger array,
 * into the array.
 */
void* array_insert(void* arr, const size_t n, const size_t size);
