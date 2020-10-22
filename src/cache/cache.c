/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "cache.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hash_table.h"
#include "heap.h"

typedef struct Cache {
  size_t limit;
  HashTable* data;
  Heap* time_tracker;
  freer freer;
} Cache;

typedef struct CacheItem {
  void* payload;
  void* key;
  size_t key_size;
  int64_t last_accessed;
} CacheItem;

static int64_t GetTimeStamp() {
  struct timespec t;

  int result = clock_gettime(CLOCK_REALTIME, &t);
  if (result != 0) {
    perror("Error generating time stamp: ");
    return 0;
  }

  return (t.tv_sec) * (int64_t)1000000000 + (int64_t)(t.tv_nsec);
}

static void CacheItem_Destory(CacheItem* ci, freer freer) {
  if (ci == NULL) return;

  free(ci->key);
  if (freer != NULL) freer(ci->payload);
  free(ci);
}

static ResultCode PurgeOldestItem(Cache* self) {
  CacheItem* ci = NULL;
  ResultCode result_code = Heap_Extract(self->time_tracker, (void**)&ci);
  if (result_code != kSuccess) return result_code;

  result_code = HashTable_Remove(self->data, ci->key, ci->key_size);
  if (result_code != kSuccess) return result_code;

  CacheItem_Destory(ci, self->freer);
  return kSuccess;
}

static ResultCode CacheItem_Create(void* payload, void* key, size_t key_size,
                                   CacheItem** result) {
  CacheItem* ci = malloc(sizeof(CacheItem));
  if (ci == NULL) return kFailedMemoryAllocation;

  ci->payload = payload;
  ci->last_accessed = GetTimeStamp();
  ci->key = malloc(key_size);
  if (ci->key == NULL) {
    CacheItem_Destory(ci, NULL);
    return kFailedMemoryAllocation;
  }

  memcpy(ci->key, key, key_size);
  ci->key_size = key_size;
  *result = ci;
  return kSuccess;
}

static int CacheItemComparator(const void* x, const void* y) {
  CacheItem* _x = (CacheItem*)x;
  CacheItem* _y = (CacheItem*)y;

  return (int)(_y->last_accessed - _x->last_accessed);
}

static void HashFreer(const KeyValuePair* x, const size_t index,
                      void* context) {
  (void)index;

  freer f = (freer)context;
  CacheItem_Destory(x->value, f);
}

ResultCode Cache_Create(size_t limit, freer freer, Cache** self) {
  if (self == NULL) return kNullParameter;
  if (*self != NULL) return kOutputPointerIsNotNull;
  if (limit == 0) return kArgumentOutOfRange;

  Cache* c = malloc(sizeof(Cache));
  if (c == NULL) return kFailedMemoryAllocation;

  c->data = NULL;
  c->time_tracker = NULL;
  c->freer = freer;
  c->limit = limit;

  ResultCode result_code = HashTable_Create(limit, &c->data);
  if (result_code != kSuccess) {
    Cache_Destroy(c);
    return result_code;
  }

  result_code = Heap_Create(limit + 1, CacheItemComparator, &c->time_tracker);
  if (result_code != kSuccess) {
    Cache_Destroy(c);
    return result_code;
  }

  *self = c;
  return kSuccess;
}

ResultCode Cache_Get(Cache* self, void* key, size_t key_size, producer producer,
                     void** result) {
  if (self == NULL || key == NULL || producer == NULL || result == NULL) {
    return kNullParameter;
  }

  if (key_size == 0) return kArgumentOutOfRange;
  if (*result != NULL) return kOutputPointerIsNotNull;

  ResultCode result_code;

  // Return the item from cache if it exists
  if (HashTable_KeyExists(self->data, key, key_size)) {
    CacheItem* ci;
    result_code = HashTable_Get(self->data, key, key_size, (void**)&ci);
    if (result_code != kSuccess) return result_code;

    // Update the last accessed variable
    ci->last_accessed = GetTimeStamp();
    Heap_Reproiritize(self->time_tracker, ci);
    *result = ci->payload;
    return kSuccess;
  }

  // Create a new cache item and add it to the cache
  CacheItem* ci = NULL;
  result_code = CacheItem_Create(producer(key, key_size), key, key_size, &ci);
  if (result_code != kSuccess) return result_code;

  result_code = HashTable_Put(self->data, key, key_size, ci);
  if (result_code != kSuccess) return result_code;

  result_code = Heap_Insert(self->time_tracker, ci);
  if (result_code != kSuccess) return result_code;

  *result = ci->payload;

  // Purge an item from the cache if there are too many
  if (HashTable_GetN(self->data) > self->limit) return PurgeOldestItem(self);

  return kSuccess;
}

void Cache_Destroy(Cache* self) {
  if (self == NULL) return;

  // Free all the existing cache items
  if (self->data != NULL && self->freer != NULL) {
    HashTable_Enumerate(self->data, HashFreer, (void*)self->freer);
  }

  HashTable_Destroy(self->data, NULL);
  Heap_Destroy(self->time_tracker, NULL);
  free(self);
}
