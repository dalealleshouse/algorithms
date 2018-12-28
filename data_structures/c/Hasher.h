/*
 * Caveats:
 * - Uses farm hash to for hash generation. The algorithm used is specifically
 *   meant for in-memory hash tables. The hashes are not guaranteed to be
 *   identical on different platforms or between executions. Therefore, this
 *   hash table should never be serialized
 */
#pragma once

#include <stddef.h>

typedef unsigned hash;

// attribution: http://www.cse.yorku.ca/~oz/hash.html
size_t compress_hash(hash, size_t);
size_t generate_index(const char*, size_t str_length, size_t max_index);
