#pragma once

#include <stdbool.h>

#include "include/ResultCode.h"

typedef unsigned int id;
typedef size_t size;
typedef unsigned long value;

typedef struct Item {
  id id;
  value value;
  size size;
} Item;

typedef struct Knapsack {
  size capacity;
  size_t n;
  Item** items;
} Knapsack;

typedef struct PackedKnapsack {
  size used_capacity;
  value total_value;
  size orginal_capacity;
  size_t n;
  Item** items;
} PackedKnapsack;

/*
 * Packs this values into a Knapsack struct
 */
ResultCode Knapsack_Init(const id ids[], const value values[],
                         const size sizes[], const size_t n,
                         const size capacity, Knapsack** knapsack);

void PackedKnapsack_Destory(PackedKnapsack* self);
void Knapsack_Destory(Knapsack* self);

/*
 * Calculates an optimal packing solutions
 *
 * params:
 *  <self>:     Knapsack that needs to be packs
 *  <result>:   Pointer providing a handle to an optimally packed knapsack. The
 *              initial value should be NULL as the function will populate it.
 */
ResultCode Knapsack_Pack(const Knapsack* self, PackedKnapsack** result);

ResultCode Knapsack_Pack_Optimal(const Knapsack* self, value* result);
