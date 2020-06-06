#include "./quick_sort.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int pivot_on_zero(const size_t n, const size_t size, const void* arr,
                  const comparator comparator) {
  (void)n;
  (void)size;
  (void)arr;
  (void)comparator;
  return 0;
}

/*
 * This is a flawed implementation because it should be returning a uniformly
 * random number and it's not. However, for the purposes of this project, it's
 * no that important
 */
int pivot_on_random(const size_t n, const size_t size, const void* arr,
                    const comparator comparator) {
  (void)size;
  (void)arr;
  (void)comparator;
  return rand() % n;
}

int pivot_on_last(const size_t n, const size_t size, const void* arr,
                  const comparator comparator) {
  (void)size;
  (void)arr;
  (void)comparator;
  if (n <= 1) return 0;

  return n - 1;
}

int pivot_on_median(const size_t n, const size_t size, const void* arr,
                    const comparator comparator) {
  if (n <= 2) return 0;

  size_t mid_point = 0;

  if (n % 2 == 0)
    mid_point = n / 2 - 1;
  else
    mid_point = n / 2;

  const void* first = arr;
  const void* last = (char*)arr + size * (n - 1);
  const void* middle = (char*)arr + size * mid_point;

  // first
  if ((comparator(first, last) >= 0 && comparator(first, middle) <= 0) ||
      (comparator(first, last) <= 0 && comparator(first, middle) >= 0))
    return 0;

  // middle
  if ((comparator(middle, first) >= 0 && comparator(middle, last) <= 0) ||
      (comparator(middle, first) <= 0 && comparator(middle, last) >= 0))
    return mid_point;

  // the only choice left is last
  return n - 1;
}

int _swap(const size_t size, void* x, void* y) {
  if (size == 0 || x == NULL || y == NULL) return -1;

  char n[size];
  memcpy(n, x, size);
  memcpy(x, y, size);
  memcpy(y, n, size);

  return 0;
}

// It is assumed that the pivot will be the first item in the array
int partition(const size_t n, const size_t size, void* arr,
              const comparator comparator, size_t* pivot_index) {
  if (arr == NULL || comparator == NULL || pivot_index == NULL || n == 0 ||
      size == 0)
    return -1;

  char* t_arr = (char*)arr;
  char* pivot_value = t_arr;
  t_arr += size;
  char* pivot_pos = t_arr;
  *pivot_index = 0;

  for (size_t i = 1; i < n; i++) {
    // If the item is less, swap it, otherwise do nothing
    if (comparator(t_arr, pivot_value) < 0) {
      if (_swap(size, t_arr, pivot_pos) < 0) return -1;

      pivot_pos += size;
      (*pivot_index)++;
    }

    t_arr += size;
  }

  if (*pivot_index > 0)
    if (_swap(size, pivot_value, pivot_value + *pivot_index * size) < 0)
      return -1;

  return 0;
}

int quick_sort(const size_t n, const size_t size, void* arr,
               const comparator comparator) {
  return quick_sort_pivot(n, size, arr, comparator, pivot_on_random);
}

int quick_sort_pivot(const size_t n, const size_t size, void* arr,
                     const comparator comparator,
                     const choose_pivot choose_pivot) {
  if (arr == NULL || comparator == NULL) return -1;

  if (n <= 1) return 0;

  size_t pivot_index;
  int pivot;

  if ((pivot = choose_pivot(n, size, arr, comparator)) < 0) return -1;

  // move the partition value to the first position
  if (_swap(size, arr, (char*)arr + pivot * size) != 0) return -2;

  if (partition(n, size, arr, comparator, &pivot_index) != 0) return -3;

  int result;
  // items to the left of the partition
  if (pivot_index > 0)
    if ((result = quick_sort_pivot(pivot_index, size, arr, comparator,
                                   choose_pivot)) != 0)
      return result;

  // items to the right of the partition
  pivot_index++;
  if (pivot_index < n)
    if ((result = quick_sort_pivot(n - pivot_index, size,
                                   (char*)arr + pivot_index * size, comparator,
                                   choose_pivot)) != 0)
      return result;

  return 0;
}
