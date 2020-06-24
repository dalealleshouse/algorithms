#include "./hash_table.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../hashing/hash_functions.h"
#include "../list_data_structures/linked_list.h"

const size_t ERROR_VAL = SIZE_MAX;
const hasher hash_func = farm_hash;
const hash_compressor compressor = mul_compressor64;

typedef struct HashTable {
  size_t m;
  size_t n;
  size_t collisons;
  LinkedList** table;
} HashTable;

// Find a valid size for the hash table
static size_t _nextPowerOf2(size_t val) {
  size_t count = 0;

  if (val && !(val & (val - 1))) return val;

  while (val != 0) {
    val >>= 1;
    count += 1;
  }

  return 1 << count;
}

static size_t generate_index(void* key, size_t key_length, size_t max_index) {
  hash h = hash_func(key, key_length);
  return compressor(h, max_index);
}

static int ele_comparator(const void* x, const void* y) {
  KeyValuePair* _x = (KeyValuePair*)x;
  KeyValuePair* _y = (KeyValuePair*)y;

  if (_x->key_length != _y->key_length) return -1;

  return memcmp(_x->key, _y->key, _x->key_length);
}

static void KeyValuePair_destroy(void* el) {
  if (el == NULL) return;

  KeyValuePair* _el = (KeyValuePair*)el;

  free(_el->key);
  free(_el);
}

static KeyValuePair* KeyValuePair_create(void* key, size_t key_length,
                                         void* value) {
  KeyValuePair* el = malloc(sizeof(KeyValuePair));
  if (el == NULL) return NULL;

  el->key = malloc(key_length);
  if (el->key == NULL) {
    KeyValuePair_destroy(el);
    return NULL;
  }

  memcpy(el->key, key, key_length);
  el->key_length = key_length;
  el->value = value;

  /* printf("%s, %s, %zu\n", el->key, key, key_length); */

  return el;
}

static KeyValuePair* search_list(LinkedList* list, void* key,
                                 size_t key_length) {
  KeyValuePair temp = {key, key_length, NULL};
  void* result = NULL;

  LinkedList_Search(list, &temp, &result);
  return result;
}

HashTable* HashTable_Create(size_t size) {
  if (size == 0) {
    ERROR("HashTable", kArgumentOutOfRange);
    return NULL;
  }

  size_t valid_size = _nextPowerOf2(size);

  HashTable* ht = calloc(sizeof(HashTable), 1);
  if (ht == NULL) {
    ERROR("HashTable", kFailedMemoryAllocation);
    return NULL;
  }

  ht->table = calloc(sizeof(void*), valid_size);
  if (ht->table == NULL) {
    ERROR("HashTable", kFailedMemoryAllocation);
    HashTable_Destroy(ht, NULL);
    return NULL;
  }

  ht->collisons = 0;
  ht->m = size;
  return ht;
}

Result HashTable_Insert(HashTable* self, void* key, size_t len, void* value) {
  if (self == NULL || key == NULL || value == NULL) return kNullParameter;

  size_t index = generate_index(key, len, self->m);

  LinkedList* ls = self->table[index];
  KeyValuePair* el = NULL;

  if (ls == NULL) {
    LinkedList_Create(KeyValuePair_destroy, ele_comparator,
                      &self->table[index]);
    if (self->table[index] == NULL) return kFailedMemoryAllocation;

    ls = self->table[index];
  } else {
    // search the list if already exists, otherwise, just leave el NULL
    el = search_list(ls, key, len);

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
    el = KeyValuePair_create(key, len, value);
    if (el == NULL) return kFailedMemoryAllocation;

    ResultCode result = LinkedList_InsertAt(ls, el, 0);
    if (result != kSuccess) return kDependancyError;
  }

  self->n++;

  return kSuccess;
}

size_t HashTable_GetN(HashTable* self) {
  if (self == NULL) {
    ERROR("HashTable", kNullParameter);
    return ERROR_VAL;
  }

  return self->n;
}

size_t HashTable_GetCollisions(HashTable* self) {
  if (self == NULL) {
    ERROR("HashTable", kNullParameter);
    return ERROR_VAL;
  }

  return self->collisons;
}

Result HashTable_Delete(HashTable* self, void* key, size_t len) {
  if (self == NULL || key == NULL) return kNullParameter;

  size_t index = generate_index(key, len, self->m);
  LinkedList* ls = self->table[index];

  if (ls == NULL) return kNotFound;

  KeyValuePair* el = search_list(ls, key, len);
  if (el == NULL) return kNotFound;

  KeyValuePair temp = {key, len, NULL};
  ResultCode result = LinkedList_Delete(ls, &temp);
  if (result != kSuccess) return kDependancyError;

  self->n--;
  return kSuccess;
}

double HashTable_GetLoadFactor(HashTable* self) {
  if (self == NULL) {
    ERROR("HashTable", kNullParameter);
    return NAN;
  }

  return (double)self->n / (double)self->m;
}

bool HashTable_KeyExists(HashTable* self, void* key, size_t len) {
  if (self == NULL || key == NULL) {
    ERROR("HashTable", kNullParameter);
    return false;
  }

  size_t index = generate_index(key, len, self->m);
  LinkedList* ls = self->table[index];
  if (ls == NULL) {
    return false;
  }

  KeyValuePair* el = search_list(ls, key, len);
  if (el == NULL) {
    return false;
  }

  return true;
}

void* HashTable_Find(HashTable* self, void* key, size_t len) {
  if (self == NULL || key == NULL) {
    ERROR("HashTable", kNullParameter);
    return NULL;
  }

  size_t index = generate_index(key, len, self->m);
  LinkedList* ls = self->table[index];
  if (ls == NULL) {
    /*     ERROR("HashTable", kNotFound); */
    return NULL;
  }

  KeyValuePair* el = search_list(ls, key, len);
  if (el == NULL) {
    /*     ERROR("HashTable", kNotFound); */
    return NULL;
  }

  return el->value;
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
