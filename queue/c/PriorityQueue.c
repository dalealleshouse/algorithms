#include <stdlib.h>

#include "PriorityQueue.h"
#include "include/MemAllocMock.h"

PriorityQueue* PQ_Create(comparator comparator)
{
    if (comparator == NULL) {
        PQ_ERROR(PQ_NullParameter);
        return NULL;
    }

    PriorityQueue* q = calloc(sizeof(PriorityQueue), 1);
    if (q == NULL) {
        PQ_ERROR(PQ_FailedMalloc);
        return NULL;
    }

    q->comparator = comparator;

    return q;
}

static void InsertHead(PriorityQueue* self, PQ_Item* item)
{
    item->next = self->head;
    self->head = item;

    if (self->tail == NULL)
        self->tail = item;
}

PQResult PQ_Insert(PriorityQueue* self, void* item)
{
    if (self == NULL || item == NULL)
        return PQ_NullParameter;

    PQ_Item* qitem = calloc(sizeof(PQ_Item), 1);
    if (qitem == NULL)
        return PQ_FailedMalloc;

    qitem->payload = item;

    if (self->n == 0)
        InsertHead(self, qitem);
    else {
        bool found = false;
        PQ_Item* prev = NULL;
        PQ_Item* i = self->head;

        while (i != NULL) {
            if (self->comparator(qitem->payload, i->payload) == 1) {
                if (prev == NULL)
                    InsertHead(self, qitem);
                else {
                    prev->next = qitem;
                    qitem->next = i;
                }
                found = true;
                break;
            }
            prev = i;
            i = i->next;
        }

        // The item should not be priortized above any other item, so place it
        // at the tail
        if (!found) {
            self->tail->next = qitem;
            self->tail = qitem;
        }
    }

    self->n++;

    return PQ_Success;
}

void* PQ_Remove(PriorityQueue* self)
{
    if (self == NULL) {
        PQ_ERROR(PQ_NullParameter);
        return NULL;
    }

    if (PQ_IsEmpty(self)) {
        PQ_ERROR(PQ_EmptyQueue);
        return NULL;
    }

    PQ_Item* qitem = self->head;
    void* item = qitem->payload;

    self->head = qitem->next;
    if (self->head == NULL)
        self->tail = NULL;

    self->n--;
    free(qitem);
    return item;
}

void* PQ_Peek(PriorityQueue* self)
{
    if (self == NULL) {
        PQ_ERROR(PQ_NullParameter);
        return NULL;
    }

    if (PQ_IsEmpty(self)) {
        PQ_ERROR(PQ_EmptyQueue);
        return NULL;
    }

    return self->head->payload;
}

bool PQ_IsEmpty(PriorityQueue* self)
{
    if (self == NULL) {
        PQ_ERROR(PQ_NullParameter);
        return true;
    }

    return self->n == 0;
}

void PQ_Destroy(PriorityQueue* self, freer freer)
{
    if (self == NULL)
        return;

    PQ_Item* curr = self->head;
    while (curr != NULL) {
        PQ_Item* temp = curr;
        curr = temp->next;
        if (freer != NULL)
            freer(temp->payload);
        free(temp);
    }
    free(self);
}

char* PQ_ErrorMessage(PQResult result)
{
    switch (result) {
    case PQ_EmptyQueue:
        return "Attempt to remove an item from an empty priority queue";
    case PQ_FailedMalloc:
        return "Failed to allocate memory";
    case PQ_NullParameter:
        return "One of the required parameters passed to the function is NULL";
    case PQ_Success:
        return "Success";
    default:
        return "Unknown error code";
    }
}
