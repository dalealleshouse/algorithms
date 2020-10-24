/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include "hash_table.h"
#include "result_code.h"

typedef struct SetItem {
  const void* payload;
  struct SetItem* parent;
  size_t rank;
} SetItem;

typedef struct DisjointSet {
  size_t num_sets;
  HashTable* items;
} DisjointSet;

/*
 * Initializes the values in the disjoint set
 *
 * params:
 *  <self>: <DisjointSet> to populate
 *  <n>:    The expected number of items in set. This value is used to set the
 *          size of the <HashTable> where the items are stored. The actual
 *          number of items may vary.
 *
 * returns:
 *  Result code indicating kSuccess/failure status
 */
ResultCode DisjointSet_Init(DisjointSet* self, const size_t n);

/*
 * Locates the sets where <x> and <y> reside and unions them if they are
 * different. This is a no-op if both items are already in the same set
 *
 * params:
 *  <self>: <DisjointSet> where items are located
 *  <x>:    The first item to locate
 *  <y>:    The second item to locate
 *
 * returns:
 *  Result code indicating kSuccess/failure status
 */
ResultCode DisjointSet_Union(DisjointSet* self, const void* x, const void* y);

/*
 * Locates the sets where <item> resides
 *
 * params:
 *  <self>: <DisjointSet> where items are located
 *  <item>: The item to search for
 *  <set>:  Pointer to populate with the address of the found set
 *
 * returns:
 *  Result code indicating kSuccess/failure status
 */
ResultCode DisjointSet_FindSet(const DisjointSet* self, const void* item,
                               SetItem** set);

/*
 * Creates a new set in the disjoint set
 *
 * params:
 *  <self>: <DisjointSet> to create a new set in
 *  <rep>:  Item that will act as the representative for the newly created set
 *  <set>:  Pointer to populate with the address of the newly created set
 *
 * returns:
 *  Result code indicating kSuccess/failure status
 */
ResultCode DisjointSet_MakeSet(DisjointSet* self, const void* rep,
                               SetItem** set);

/*
 * Cleans up memory allocations within the disjoint set. The consumer is
 * responsible for cleaning up the set itself as well as any items passed in to
 * DisjointSet_MakeSet.
 */
void DisjointSet_Destory(DisjointSet*);
