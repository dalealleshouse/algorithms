/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>

typedef enum { kQuickSelect = 1, kSortSelect = 2 } SelectAlgorithm;

double SelectTime(const size_t n, const size_t nth, const SelectAlgorithm algo);
