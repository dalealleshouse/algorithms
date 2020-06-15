#include "./priority_queue.h"

#include <stdlib.h>

PriorityQueue* PQ_Create(comparator comparator) {
  if (comparator == NULL) {
    PQ_ERROR(PQ_kNullParameter);
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

static void InsertHead(PriorityQueue* self, PQ_Item* item) {
  if (self->head == NULL) {
    self->head = item;
    self->tail = item;
    return;
  }

  PQ_Item* middle = item;
  PQ_Item* right = self->head;

  middle->next = right;
  right->prev = middle;
  self->head = middle;
}

static void InsertTail(PriorityQueue* self, PQ_Item* item) {
  PQ_Item* left = self->tail;
  PQ_Item* middle = item;

  left->next = middle;
  middle->prev = left;
  self->tail = middle;
}

static void* PQ_RemoveTail(PriorityQueue* self) {
  PQ_Item* qitem = self->tail;
  void* item = qitem->payload;

  self->tail = qitem->prev;
  if (self->tail == NULL) {
    self->head = NULL;
  } else {
    self->tail->next = NULL;
  }

  self->n--;
  free(qitem);
  return item;
}

static void* PQ_RemoveMid(PriorityQueue* self, PQ_Item* item) {
  PQ_Item* left = item->prev;
  PQ_Item* right = item->next;

  left->next = right;
  right->prev = left;

  void* i = item->payload;
  self->n--;
  free(item);
  return i;
}

static void InsertMid(PQ_Item* item, PQ_Item* after) {
  // You could do this a bit more efficently, but this makes it more
  // understandable
  PQ_Item* left = after->prev;
  PQ_Item* middle = item;
  PQ_Item* right = after;

  left->next = middle;
  middle->prev = left;
  middle->next = right;
  right->prev = middle;
}

PQResult PQ_Insert(PriorityQueue* self, void* item) {
  if (self == NULL || item == NULL) return PQ_kNullParameter;

  PQ_Item* qitem = calloc(sizeof(PQ_Item), 1);
  if (qitem == NULL) return PQ_FailedMalloc;

  qitem->payload = item;

  if (self->n == 0) {
    InsertHead(self, qitem);
  } else {
    bool found = false;
    PQ_Item* i = self->head;

    while (i != NULL) {
      if (self->comparator(qitem->payload, i->payload) >= 0) {
        if (i->prev == NULL) {
          InsertHead(self, qitem);
        } else {
          InsertMid(qitem, i);
        }

        found = true;
        break;
      }
      i = i->next;
    }

    // The item should not be priortized above any other item, so place it
    // at the tail
    if (!found) InsertTail(self, qitem);
  }

  self->n++;

  return PQ_kSuccess;
}

static PQ_Item* FindItem(PriorityQueue* self, void* item) {
  PQ_Item* i = self->head;

  while (i != NULL) {
    if (i->payload == item) return i;

    i = i->next;
  }

  return NULL;
}

PQResult PQ_Reprioritize(PriorityQueue* self, void* item) {
  if (self == NULL || item == NULL) return PQ_kNullParameter;

  PQ_Item* i = FindItem(self, item);
  if (i == NULL) return PQ_ItemkNotFound;

  if (i == self->head) {
    void* x = PQ_Remove(self);
    return PQ_Insert(self, x);
  }

  if (i == self->tail) {
    void* x = PQ_RemoveTail(self);
    return PQ_Insert(self, x);
  }

  void* x = PQ_RemoveMid(self, i);
  return PQ_Insert(self, x);

  return PQ_kSuccess;
}

void* PQ_Remove(PriorityQueue* self) {
  if (self == NULL) {
    PQ_ERROR(PQ_kNullParameter);
    return NULL;
  }

  if (PQ_IskEmpty(self)) {
    PQ_ERROR(PQ_kEmptyQueue);
    return NULL;
  }

  PQ_Item* qitem = self->head;
  void* item = qitem->payload;

  self->head = qitem->next;
  if (self->head == NULL) {
    self->tail = NULL;
  } else {
    self->head->prev = NULL;
  }

  self->n--;
  free(qitem);
  return item;
}

void* PQ_Peek(PriorityQueue* self) {
  if (self == NULL) {
    PQ_ERROR(PQ_kNullParameter);
    return NULL;
  }

  if (PQ_IskEmpty(self)) {
    PQ_ERROR(PQ_kEmptyQueue);
    return NULL;
  }

  return self->head->payload;
}

bool PQ_IskEmpty(PriorityQueue* self) {
  if (self == NULL) {
    PQ_ERROR(PQ_kNullParameter);
    return true;
  }

  return self->n == 0;
}

void PQ_Destroy(PriorityQueue* self, freer freer) {
  if (self == NULL) return;

  PQ_Item* curr = self->head;
  while (curr != NULL) {
    PQ_Item* temp = curr;
    curr = temp->next;
    if (freer != NULL) freer(temp->payload);
    free(temp);
  }
  free(self);
}

char* PQ_ErrorMessage(PQResult result) {
  switch (result) {
    case PQ_ItemkNotFound:
      return "Unable to find the specified item";
    case PQ_kEmptyQueue:
      return "Attempt to remove an item from an empty priority queue";
    case PQ_FailedMalloc:
      return "Failed to allocate memory";
    case PQ_kNullParameter:
      return "One of the required parameters passed to the function is NULL";
    case PQ_kSuccess:
      return "kSuccess";
    default:
      return "Unknown error code";
  }
}
