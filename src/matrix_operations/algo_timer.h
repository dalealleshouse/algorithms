/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stddef.h>

typedef enum {
  kNaiveMatrixMultiply = 1,
  kTilingMatrixMultiply = 2,
  kStrassenMatrixMultiply = 3,
  kTransposeMulitply = 4,
  kRecursiveMultiply = 5
} MatrixAlgoirthm;

double MatrixMultiplyTime(const size_t n, const MatrixAlgoirthm algo);
