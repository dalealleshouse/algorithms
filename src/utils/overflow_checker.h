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
bool IsAddOverflow_size_t(size_t x, size_t y);
bool IsAddOverflow_int(int x, int y);
bool IsAddOverflow_uint(unsigned int x, unsigned int y);
bool IsAddOverflow_ulong(unsigned long x, unsigned long y);
bool IsMulOverflow_ulong(unsigned long x, unsigned long y);
