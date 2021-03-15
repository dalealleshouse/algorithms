/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stdlib.h>

#include "comparators.h"
#include "result_code.h"

ResultCode CountInversions(const void* arr, const size_t length,
                           const size_t item_size, sort_strategy comparator,
                           size_t* result);
