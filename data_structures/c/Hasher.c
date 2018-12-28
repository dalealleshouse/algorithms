#include <math.h>
#include <stdio.h>
#include <string.h>

#include "Hasher.h"
#include "farmhash.h"

// NOT optimized to take advantage of machine architecture
size_t compress_hash(hash k, size_t m)
{
    const double A = (sqrt(5) - 1) / 2;
    return m * fmodl(k * A, 1);
}

size_t generate_index(const char* str, size_t str_length, size_t m)
{
    hash result = farmhash(str, str_length);
    return compress_hash(result, m);
}
