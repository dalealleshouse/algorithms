#ifndef __MERGE_SORT_H
#define __MERGE_SORT_H

#include <stdlib.h>

/*
 * returns:
 *  < 0 if x goes before y
 *  1 if x = y
 *  > 0 if x goes after y
 */
typedef int (*comparator)(const void* x, const void* y);

int sort(void* arr, void* output, size_t length, size_t item_size,
    comparator comparator);

#endif
