#pragma once

#include <stdbool.h>

#include "result_code.h"

typedef long variable;
typedef struct clause {
  variable a;
  variable b;
} clause;

ResultCode Sat_EvalClause(clause* clause, bool a, bool b, bool* satified);
ResultCode Sat_Solve(size_t n, clause clauses[n], bool* result);
