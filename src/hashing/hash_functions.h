/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>
#include <stdint.h>

typedef uint64_t hash;
typedef hash (*hasher)(void*, size_t);
typedef size_t (*hash_compressor)(hash, size_t);

const size_t hasher_count;
const hasher hashers[14];

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
size_t mul_compressor64(hash, size_t);
