/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stdbool.h>
#include <stddef.h>

bool IsMulOverflow_size_t(size_t x, size_t y);
bool is_add_overflow_size_t(size_t x, size_t y);
bool is_add_overflow_int(int x, int y);
bool is_add_overflow_uint(unsigned int x, unsigned int y);
bool is_add_overflow_ulong(unsigned long x, unsigned long y);
bool is_mul_overflow_ulong(unsigned long x, unsigned long y);
