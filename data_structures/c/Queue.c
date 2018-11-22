#include <stdlib.h>

#include "Queue.h"
#include "include/MemAllocMock.h"

Queue* Queue_Create()
{
    Queue* queue = calloc(sizeof(Queue), 1);
    if (queue == NULL)
        return NULL;

    return queue;
}

QueueResult Queue_Enqueue(Queue* self, void* item)
{
    if (self == NULL || item == NULL)
        return Queue_NullParameter;

    Queue_Item* q_item = calloc(sizeof(Queue_Item), 1);

    if (q_item == NULL)
        return Queue_FailedMalloc;

    q_item->payload = item;
    if (self->n == 0) {
        self->head = q_item;
        self->tail = q_item;
    } else {
        self->tail->next = q_item;
        self->tail = q_item;
    }

    self->n++;

    return Queue_Success;
}

void* Queue_Dequeue(Queue* self)
{
    if (self == NULL || self->n == 0)
        return NULL;

    void* item = self->head->payload;
    if (self->n == 1) {
        free(self->head);
        self->head = NULL;
        self->tail = NULL;
    } else {
        Queue_Item* doomed = self->head;
        self->head = doomed->next;
        free(doomed);
    }

    self->n--;
    return item;
}

bool Queue_IsEmpty(Queue* self)
{
    if (self == NULL)
        return true;

    return self->n == 0;
}

void Queue_Destroy(Queue* self)
{
    if (self == NULL)
        return;

    Queue_Item* curr = self->head;
    while (curr != NULL) {
        Queue_Item* temp = curr;
        curr = temp->next;
        free(temp);
    }

    free(self);
}
