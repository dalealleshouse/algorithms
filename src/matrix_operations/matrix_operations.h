/*******************************************************************************
 * Copyright (C) 2021 Dale Alleshouse (AKA Hideous Humpback Freak)
 *  dale@alleshouse.net https://hideoushumpbackfreak.com/
 *
 * This file is subject to the terms and conditions defined in the 'LICENSE'
 * file, which is part of this source code package.
 ******************************************************************************/
#pragma once

#include <stdint.h>

#include "result_code.h"

typedef int64_t matrix_value;

ResultCode Matrix_Initalize(size_t n, matrix_value (**matrix)[n][n]);
ResultCode Matrices_Initalize(const size_t n, const size_t count, ...);
void Matrices_Destroy(const size_t count, ...);

ResultCode Matrix_Add(size_t n, matrix_value (*x)[n][n],
                      matrix_value (*y)[n][n], matrix_value (*result)[n][n]);

ResultCode Matrix_Subtract(size_t n, matrix_value (*x)[n][n],
                           matrix_value (*y)[n][n],
                           matrix_value (*result)[n][n]);

ResultCode Matrix_Multiply(size_t n, matrix_value (*x)[n][n],
                           matrix_value (*y)[n][n],
                           matrix_value (*result)[n][n]);

ResultCode Matrix_StrassenMultiply(size_t n, matrix_value (*x)[n][n],
                                   matrix_value (*y)[n][n],
                                   matrix_value (*result)[n][n]);
