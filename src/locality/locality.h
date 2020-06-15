// Copyright 2020 Dale Alleshouse
#pragma once

#include <stdint.h>

#include "../utils/result_code.h"

ResultCode ColumnOrder(size_t n, int64_t array[n][n]);
ResultCode RowOrder(size_t n, int64_t array[n][n]);
