#include "merge_sort.h"

void copy_bytes(char* dest, char* source, size_t num_bytes)
{
    for (size_t i = 0; i < num_bytes; i++) {
        *dest = *source;
        dest++;
        source++;
    }
}

int sort(void* arr, void* output, size_t length, size_t item_size,
    comparator comparator)
{
    if (arr == NULL || output == NULL)
        return -1;

    if (length > 1) {
        // If length is odd, this will assign the larger half to b.
        size_t a_length = length / 2;
        size_t b_length = length - a_length;

        size_t a_size = a_length * item_size;
        size_t b_size = b_length * item_size;

        char a[a_size];
        char b[b_size];

        sort(arr, a, a_length, item_size, comparator);
        sort(arr + a_size, b, b_length, item_size, comparator);

        size_t a_pos = 0;
        size_t b_pos = 0;
        for (size_t i = 0; i < length; i++) {
            if (a_pos >= a_size) {
                copy_bytes(output, &b[b_pos], item_size);
                b_pos += item_size;
            } else if (b_pos >= b_size) {
                copy_bytes(output, &a[a_pos], item_size);
                a_pos += item_size;
            } else {
                int result = comparator(&a[a_pos], &b[b_pos]);

                if (result < 0) {
                    copy_bytes(output, &a[a_pos], item_size);
                    a_pos += item_size;
                } else {
                    copy_bytes(output, &b[b_pos], item_size);
                    b_pos += item_size;
                }
            }

            output += item_size;
        }
    } else {
        copy_bytes(output, arr, item_size);
    }

    return 0;
}
