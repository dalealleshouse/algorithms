/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "hash_table.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common_math.h"
#include "hash_functions.h"
#include "linked_list.h"

const hasher hash_func = farm_hash;
const hash_compressor compressor = mul_compressor64;

typedef struct HashTable {
  size_t m;
  size_t n;
  size_t collisons;
  LinkedList** table;
} HashTable;

static size_t GenerateIndex(void* key, size_t key_length, size_t max_index) {
  hash h = hash_func(key, key_length);
  return compressor(h, max_index);
}

static int ElementComparator(const void* x, const void* y) {
  KeyValuePair* _x = (KeyValuePair*)x;
  KeyValuePair* _y = (KeyValuePair*)y;

  if (_x->key_length != _y->key_length) return -1;

  return memcmp(_x->key, _y->key, _x->key_length);
}

static void KeyValuePair_Destroy(void* el) {
  if (el == NULL) return;

  KeyValuePair* _el = (KeyValuePair*)el;

  free(_el->key);
  free(_el);
}

static KeyValuePair* KeyValuePair_Create(void* key, size_t key_length,
                                         void* value) {
  KeyValuePair* el = malloc(sizeof(KeyValuePair));
  if (el == NULL) return NULL;

  el->key = malloc(key_length);
  if (el->key == NULL) {
    KeyValuePair_Destroy(el);
    return NULL;
  }

  memcpy(el->key, key, key_length);
  el->key_length = key_length;
  el->value = value;

  return el;
}

static KeyValuePair* SearchList(LinkedList* list, void* key,
                                size_t key_length) {
  KeyValuePair temp = {key, key_length, NULL};
  void* result = NULL;

  LinkedList_Search(list, &temp, &result);
  return result;
}

ResultCode HashTable_Create(size_t size, HashTable** self) {
  if (size == 0) return kArgumentOutOfRange;

  size_t valid_size = NextPowerOfTwo(size);

  HashTable* ht = calloc(sizeof(HashTable), 1);
  if (ht == NULL) return kFailedMemoryAllocation;

  ht->table = calloc(sizeof(void*), valid_size);
  if (ht->table == NULL) {
    HashTable_Destroy(ht, NULL);
    return kFailedMemoryAllocation;
  }

  ht->collisons = 0;
  ht->m = size;
  *self = ht;
  return kSuccess;
}

Result HashTable_Put(HashTable* self, void* key, size_t len, void* value) {
  if (self == NULL || key == NULL || value == NULL) return kNullParameter;

  size_t index = GenerateIndex(key, len, self->m);

  LinkedList* ls = self->table[index];
  KeyValuePair* el = NULL;

  if (ls == NULL) {
    LinkedList_Create(KeyValuePair_Destroy, ElementComparator,
                      &self->table[index]);
    if (self->table[index] == NULL) return kFailedMemoryAllocation;

    ls = self->table[index];
  } else {
    // search the list if already exists, otherwise, just leave el NULL
    el = SearchList(ls, key, len);

    // if the list was found, but the item doesn't exit, it's a collision
    if (el == NULL) {
      self->collisons++;
    } else {
      el->value = value;
      return kSuccess;
    }
  }

  // Insert the value if it exists, otherwise update it
  if (el == NULL) {
    el = KeyValuePair_Create(key, len, value);
    if (el == NULL) return kFailedMemoryAllocation;

    ResultCode result = LinkedList_InsertAt(ls, el, 0);
    if (result != kSuccess) return kDependancyError;
  }

  self->n++;

  return kSuccess;
}

size_t HashTable_GetN(HashTable* self) {
  if (self == NULL) return 0;

  return self->n;
}

size_t HashTable_GetCollisions(HashTable* self) {
  if (self == NULL) return 0;

  return self->collisons;
}

Result HashTable_Remove(HashTable* self, void* key, size_t len) {
  if (self == NULL || key == NULL) return kNullParameter;

  size_t index = GenerateIndex(key, len, self->m);
  LinkedList* ls = self->table[index];

  if (ls == NULL) return kNotFound;

  KeyValuePair* el = SearchList(ls, key, len);
  if (el == NULL) return kNotFound;

  KeyValuePair temp = {key, len, NULL};
  ResultCode result = LinkedList_Delete(ls, &temp);
  if (result != kSuccess) return kDependancyError;

  self->n--;
  return kSuccess;
}

double HashTable_GetLoadFactor(HashTable* self) {
  if (self == NULL) return NAN;

  return (double)self->n / (double)self->m;
}

bool HashTable_KeyExists(HashTable* self, void* key, size_t len) {
  if (self == NULL || key == NULL) return false;

  size_t index = GenerateIndex(key, len, self->m);
  LinkedList* ls = self->table[index];
  if (ls == NULL) {
    return false;
  }

  KeyValuePair* el = SearchList(ls, key, len);
  if (el == NULL) {
    return false;
  }

  return true;
}

ResultCode HashTable_Get(HashTable* self, void* key, size_t len,
                         void** result) {
  if (self == NULL || key == NULL) return kNullParameter;

  size_t index = GenerateIndex(key, len, self->m);
  LinkedList* ls = self->table[index];
  if (ls == NULL) return kNotFound;

  KeyValuePair* el = SearchList(ls, key, len);
  if (el == NULL) return kNotFound;

  *result = el->value;
  return kSuccess;
}

Result HashTable_Enumerate(HashTable* self, keyvaluehandler func,
                           void* context) {
  if (self == NULL || func == NULL) return kNullParameter;

  size_t index = 0;
  for (size_t i = 0; i < self->m; i++) {
    if (self->table[i] == NULL) continue;

    LinkedList* list = self->table[i];

    LinkedListItem* item = list->head;
    while (item != NULL) {
      KeyValuePair* el = (KeyValuePair*)item->payload;
      func(el, index, context);
      ++index;
      item = item->next;
    }
  }
  return kSuccess;
}

void HashTable_Destroy(HashTable* self, freer freer) {
  if (self == NULL) return;

  for (size_t i = 0; i < self->m; i++) {
    if (self->table[i] == NULL) continue;

    LinkedList* list = self->table[i];

    if (freer != NULL) {
      LinkedListItem* item = list->head;
      while (item != NULL) {
        KeyValuePair* el = (KeyValuePair*)item->payload;
        freer(el->value);
        item = item->next;
      }
    }

    LinkedList_Destroy(self->table[i]);
  }

  free(self->table);
  free(self);
}
