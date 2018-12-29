#include <math.h>
#include <stdio.h>

#include "HashFunctions.h"
#include "farmhash.h"
#include "spooky.h"

static const hash seed_1 = 1480248131514916162;
static const hash seed_2 = 4129361492271981247;
static const hash seed_3 = 2522131261608258122;

hash rotating_hash(void* key, size_t len)
{
    unsigned char* p = key;
    hash h = 0;

    for (size_t i = 0; i < len; i++) {
        h = (h << 4) ^ (h >> 28) ^ p[i];
    }

    return h;
}

hash djb_hash(void* key, size_t len)
{
    unsigned char* p = key;
    hash h = 0;

    for (size_t i = 0; i < len; i++) {
        h = 33 * h + p[i];
    }

    return h;
}

hash mod_djb_hash(void* key, size_t len)
{
    unsigned char* p = key;
    hash h = 0;

    for (size_t i = 0; i < len; i++) {
        h = 33 * h ^ p[i];
    }

    return h;
}

hash sax_hash(void* key, size_t len)
{
    unsigned char* p = key;
    hash h = 0;

    for (size_t i = 0; i < len; i++) {
        h ^= (h << 5) + (h >> 2) + p[i];
    }

    return h;
}

hash fnv_hash(void* key, size_t len)
{
    unsigned char* p = key;
    hash h = 2166136261;

    for (size_t i = 0; i < len; i++) {
        h = (h * 16777619) ^ p[i];
    }

    return h;
}

hash oat_hash(void* key, size_t len)
{
    unsigned char* p = key;
    hash h = 0;

    for (size_t i = 0; i < len; i++) {
        h += p[i];
        h += (h << 10);
        h ^= (h >> 6);
    }

    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);

    return h;
}

hash elf_hash(void* key, size_t len)
{
    unsigned char* p = key;
    hash h = 0, g;

    for (size_t i = 0; i < len; i++) {
        h = (h << 4) + p[i];
        g = h & 0xf000000000000000L;

        if (g != 0) {
            h ^= g >> 24;
        }

        h &= ~g;
    }

    return h;
}

hash farm_hash(void* key, size_t len) { return farmhash64(key, len); }

hash farm_hash2(void* key, size_t len)
{
    return farmhash64_with_seed(key, len, seed_2);
}

hash farm_hash3(void* key, size_t len)
{
    return farmhash64_with_seed(key, len, seed_3);
}

hash spooky_hash(void* key, size_t len)
{
    return spooky_hash64(key, len, seed_1);
}

hash spooky_hash2(void* key, size_t len)
{
    return spooky_hash64(key, len, seed_2);
}

hash spooky_hash3(void* key, size_t len)
{
    return spooky_hash64(key, len, seed_3);
}

size_t div_compressor(hash hash, size_t max_val) { return hash % max_val; }

// Specifically designed for 64 bit keys and max_val as a power of 2
size_t mul_compressor(hash key, size_t max_val)
{
    const size_t w = 64;
    const long double A = (sqrt(5) - 1) / 2;
    const hash s = A * pow(2, w);
    const size_t r = log2(max_val);

    return (s * key) >> (w - r);
}
