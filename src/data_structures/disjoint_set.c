/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "disjoint_set.h"

#include <stdlib.h>

static void SetItem_Destory(void* item) { free(item); }

Result DisjointSet_Init(DisjointSet* self, const size_t n) {
  if (self == NULL) return kNullParameter;

  if (n <= 1) return kArgumentOutOfRange;

  self->items = HashTable_Create(n);
  if (self->items == NULL) return kFailedMemoryAllocation;

  self->num_sets = 0;

  return kSuccess;
}

Result DisjointSet_MakeSet(DisjointSet* self, const void* rep, SetItem** set) {
  if (self == NULL || rep == NULL || set == NULL) return kNullParameter;

  bool exists = HashTable_KeyExists(self->items, &rep, sizeof(void*));
  if (exists) return kDuplicate;

  SetItem* item = calloc(sizeof(SetItem), 1);
  if (item == NULL) return kFailedMemoryAllocation;

  item->parent = item;
  item->payload = rep;
  ++self->num_sets;

  Result result = HashTable_Insert(self->items, &rep, sizeof(void*), item);
  if (result != kSuccess) {
    SetItem_Destory(item);
    return result;
  }

  *set = item;
  return kSuccess;
}

Result DisjointSet_FindSet(const DisjointSet* self, const void* item,
                           SetItem** set) {
  if (self == NULL || item == NULL || set == NULL) return kNullParameter;

  SetItem* _set = HashTable_Find(self->items, &item, sizeof(void*));
  if (_set == NULL) return kNotFound;

  if (_set != _set->parent) {
    // Path compression - go ahead and set to the immediate parent to
    // shorten the path for future look ups
    Result result =
        DisjointSet_FindSet(self, _set->parent->payload, &_set->parent);
    if (result != kSuccess) return result;
  }

  *set = _set->parent;
  return kSuccess;
}

Result DisjointSet_Union(DisjointSet* self, const void* x, const void* y) {
  if (self == NULL || x == NULL || y == NULL) return kNullParameter;

  SetItem* x_set = NULL;
  SetItem* y_set = NULL;

  Result result = DisjointSet_FindSet(self, x, &x_set);
  if (result != kSuccess) return result;

  result = DisjointSet_FindSet(self, y, &y_set);
  if (result != kSuccess) return result;

  if (x_set == y_set) return kSuccess;

  if (x_set->rank > y_set->rank) {
    y_set->parent = x_set;
  } else {
    x_set->parent = y_set;
    if (x_set->rank == y_set->rank) ++y_set->rank;
  }

  --self->num_sets;
  return kSuccess;
}

void DisjointSet_Destory(DisjointSet* self) {
  if (self == NULL) return;

  HashTable_Destroy(self->items, SetItem_Destory);
}
