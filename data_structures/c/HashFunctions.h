#pragma once

#include <stddef.h>
#include <stdint.h>

typedef uint64_t hash;
typedef hash (*hasher)(void*, size_t);
typedef size_t (*hash_compressor)(hash, size_t);

hash rotating_hash(void*, size_t);
hash djb_hash(void*, size_t);
hash mod_djb_hash(void*, size_t);
hash sax_hash(void*, size_t);
hash fnv_hash(void*, size_t);
hash oat_hash(void*, size_t);
hash jsw_hash(void*, size_t);
hash elf_hash(void*, size_t);
hash farm_hash(void*, size_t);
hash farm_hash2(void*, size_t);
hash farm_hash3(void*, size_t);
hash spooky_hash(void*, size_t);
hash spooky_hash2(void*, size_t);
hash spooky_hash3(void*, size_t);

size_t div_compressor(hash, size_t);

// Specifically designed for 64 bit keys and max_val as a power of 2
size_t mul_compressor(hash, size_t);
