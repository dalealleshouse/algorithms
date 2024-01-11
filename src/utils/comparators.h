/*******************************************************************************
 * Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

typedef int (*sort_strategy)(const void* x, const void* y);
typedef void (*freer)(void* x);

// Compators
int PIntComparator(const void* x, const void* y);
