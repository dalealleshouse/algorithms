/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#include "comparators.h"

// TODO(dalealleshouse): Check for NULL values
int PIntComparator(const void* x, const void* y) { return *(int*)x - *(int*)y; }
