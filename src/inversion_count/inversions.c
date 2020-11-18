#include "./inversions.h"

#include <string.h>

unsigned long _count_inversions(const void* arr, void* output,
                                const size_t length, const size_t item_size,
                                sort_strategy comparator) {
  unsigned long inversions = 0;

  if (arr == NULL || output == NULL) return -1;

  if (length == 1) {
    memcpy(output, arr, item_size);
  } else {
    // If length is odd, this will assign the larger half to b.
    size_t a_length = length / 2;
    size_t b_length = length - a_length;

    size_t a_size = a_length * item_size;
    size_t b_size = b_length * item_size;

    char a[a_size];
    char b[b_size];

    inversions += _count_inversions(arr, a, a_length, item_size, comparator);
    inversions += _count_inversions((char*)arr + a_size, b, b_length, item_size,
                                    comparator);

    size_t a_pos = 0;
    size_t b_pos = 0;
    for (size_t i = 0; i < length; i++) {
      if (a_pos >= a_size) {
        memcpy(output, &b[b_pos], b_size - b_pos);
        break;
      } else if (b_pos >= b_size) {
        memcpy(output, &a[a_pos], a_size - a_pos);
        break;
      } else {
        int result = comparator(&a[a_pos], &b[b_pos]);

        if (result <= 0) {
          memcpy(output, &a[a_pos], item_size);
          a_pos += item_size;
        } else {
          memcpy(output, &b[b_pos], item_size);

          // 1 inversion for each item left in the first half
          inversions += (a_size - a_pos) / item_size;
          b_pos += item_size;
        }
      }

      output = (char*)output + item_size;
    }
  }

  return inversions;
}

unsigned long count_inversions(const void* arr, const size_t length,
                               const size_t item_size,
                               sort_strategy comparator) {
  char sorted[length * item_size];
  return _count_inversions(arr, sorted, length, item_size, comparator);
}
