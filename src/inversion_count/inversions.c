/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "inversions.h"

#include <string.h>

static size_t CountInversionsRecursive(const void* arr, void* output,
                                       const size_t length,
                                       const size_t item_size,
                                       sort_strategy comparator) {
  size_t inversions = 0;

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

    inversions +=
        CountInversionsRecursive(arr, a, a_length, item_size, comparator);
    inversions += CountInversionsRecursive((char*)arr + a_size, b, b_length,
                                           item_size, comparator);

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

ResultCode CountInversions(const void* arr, const size_t length,
                           const size_t item_size, sort_strategy comparator,
                           size_t* result) {
  if (arr == NULL) return kNullParameter;
  if (item_size == 0 || length == 0) return kArgumentOutOfRange;

  char sorted[length * item_size];
  *result =
      CountInversionsRecursive(arr, sorted, length, item_size, comparator);
  return kSuccess;
}
