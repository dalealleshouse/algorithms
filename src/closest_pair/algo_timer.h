/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>

typedef enum { kNaiveClosestPair = 1, kClosestPair = 2 } ClosestPairAlgoirthm;

double ClosestPairTime(const size_t n, const ClosestPairAlgoirthm algo);
