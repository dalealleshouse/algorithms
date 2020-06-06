#include "./knapsack_problem.h"

#include <stdlib.h>
#include <string.h>

static ResultCode _itemInit(id id, value value, size size, Item** item) {
  if (item == NULL) return NullParameter;
  if (*item != NULL) return OutputPointerIsNotNull;

  Item* _item = malloc(sizeof(Item));
  if (_item == NULL) return FailedMemoryAllocation;

  _item->id = id;
  _item->value = value;
  _item->size = size;

  *item = _item;
  return Success;
}

static void _itemDestroy(Item* item) { free(item); }

static void _printSolutions(size_t n, size capacity,
                            value solutions[n + 1][capacity + 1]) {
  printf("\n");
  for (int c = capacity; c >= 0; c--) {
    printf("%d ", c);
    for (size_t i = 0; i <= n; i++) {
      printf(" %zu ", solutions[i][c]);
    }
    printf("\n");
  }

  printf("  ");
  for (size_t i = 0; i <= n; i++) {
    printf(" %zu ", i);
  }
  printf("\n");
}

ResultCode _packSolution(const Knapsack* orginal, const size_t n,
                         Item* items[n], size usedCapacity, value totalValue,
                         PackedKnapsack** packed) {
  if (orginal == NULL || items == NULL || packed == NULL) return NullParameter;
  if (*packed != NULL) return OutputPointerIsNotNull;

  PackedKnapsack* _knapsack = malloc(sizeof(PackedKnapsack));
  if (_knapsack == NULL) return FailedMemoryAllocation;

  _knapsack->used_capacity = usedCapacity;
  _knapsack->total_value = totalValue;
  _knapsack->orginal_capacity = orginal->capacity;
  _knapsack->n = n;

  if (n > 0) {
    _knapsack->items = calloc(sizeof(void*), n);
    if (_knapsack->items == NULL) {
      PackedKnapsack_Destory(_knapsack);
      return FailedMemoryAllocation;
    }

    memcpy(_knapsack->items, items, sizeof(void*) * n);
  }

  *packed = _knapsack;
  return Success;
}

static ResultCode _reconstruct(
    const Knapsack* knapsack,
    value solutions[knapsack->n + 1][knapsack->capacity + 1],
    PackedKnapsack** solution) {
  if (knapsack == NULL || solutions == NULL || solution == NULL) {
    return NullParameter;
  }
  if (*solution != NULL) return OutputPointerIsNotNull;

  Item* items[knapsack->n];
  value runningTotal = 0;
  size numItems = 0;
  size remainingCapacity = knapsack->capacity;

  for (size i = knapsack->n; i >= 1; i--) {
    Item* item = knapsack->items[i - 1];
    if (item->size <= remainingCapacity &&
        solutions[i - 1][remainingCapacity - item->size] + item->value >=
            solutions[i - 1][remainingCapacity]) {
      items[numItems] = item;
      runningTotal += item->value;
      ++numItems;
      remainingCapacity = remainingCapacity - item->size;
    }
  }

  return _packSolution(knapsack, numItems, items,
                       knapsack->capacity - remainingCapacity, runningTotal,
                       solution);
}

ResultCode Knapsack_Init(const id ids[], const value values[],
                         const size sizes[], const size_t n,
                         const size capacity, Knapsack** knapsack) {
  if (ids == NULL || values == NULL || sizes == NULL || knapsack == NULL) {
    return NullParameter;
  }
  if (n == 0 || capacity == 0) return ArgumentOutOfRange;
  if (*knapsack != NULL) return OutputPointerIsNotNull;

  Knapsack* _knapsack = malloc(sizeof(Knapsack));
  if (_knapsack == NULL) return FailedMemoryAllocation;

  _knapsack->capacity = capacity;
  _knapsack->n = n;

  _knapsack->items = calloc(sizeof(void*), n);
  if (_knapsack->items == NULL) {
    Knapsack_Destory(_knapsack);
    return FailedMemoryAllocation;
  }

  for (size_t i = 0; i < n; i++) {
    ResultCode r = _itemInit(ids[i], values[i], sizes[i], &_knapsack->items[i]);
    if (r != Success) {
      Knapsack_Destory(_knapsack);
      return r;
    }
  }

  *knapsack = _knapsack;
  return Success;
}

void Knapsack_Destory(Knapsack* self) {
  if (self == NULL) return;

  if (self->items != NULL) {
    for (size_t i = 0; i < self->n; i++) _itemDestroy(self->items[i]);
  }

  free(self->items);
  free(self);
}

void PackedKnapsack_Destory(PackedKnapsack* self) {
  if (self == NULL) return;
  free(self->items);
  free(self);
}

ResultCode Knapsack_Pack(const Knapsack* self, PackedKnapsack** result) {
  if (self == NULL || result == NULL) return NullParameter;
  if (*result != NULL) return OutputPointerIsNotNull;
  if (self->n == 0 || self->capacity == 0) return ArgumentOutOfRange;

  value solutions[self->n + 1][self->capacity + 1];
  memset(solutions, 0, sizeof(solutions));

  for (size_t i = 1; i <= self->n; i++) {
    Item* item = self->items[i - 1];
    for (size_t c = 0; c <= self->capacity; c++) {
      if (item->size > c) {
        solutions[i][c] = solutions[i - 1][c];
      } else {
        value case1 = solutions[i - 1][c];
        value case2 = solutions[i - 1][c - item->size] + item->value;
        if (case1 > case2) {
          solutions[i][c] = case1;
        } else {
          solutions[i][c] = case2;
        }
      }
    }
  }

  (void)_printSolutions;
  /* _printSolutions(self->n, self->capacity, solutions); */
  return _reconstruct(self, solutions, result);
}

ResultCode Knapsack_Pack_Optimal(const Knapsack* self, value* result) {
  if (self == NULL || result == NULL) return NullParameter;
  if (self->n == 0 || self->capacity == 0) return ArgumentOutOfRange;

  size_t solution_size = (self->capacity + 1) * sizeof(value);
  value* solutions = calloc(1, solution_size);
  value* my_solutions = malloc(solution_size);

  for (size_t i = 1; i <= self->n; i++) {
    Item* item = self->items[i - 1];
    for (size_t c = 0; c <= self->capacity; c++) {
      if (item->size > c) {
        my_solutions[c] = solutions[c];
      } else {
        value case1 = solutions[c];
        value case2 = solutions[c - item->size] + item->value;
        if (case1 > case2) {
          my_solutions[c] = case1;
        } else {
          my_solutions[c] = case2;
        }
      }
    }
    value* temp = my_solutions;
    my_solutions = solutions;
    solutions = temp;
  }

  *result = solutions[self->capacity];
  free(solutions);
  free(my_solutions);
  return Success;
}
