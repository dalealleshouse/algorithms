// Copyright 2020 Dale Alleshouse
#pragma once

#include <stddef.h>

typedef enum { kColumnOrder = 1, kRowOrder = 2 } IterationType;

double EnumerateMultiDimArray(const size_t n, const IterationType type);
