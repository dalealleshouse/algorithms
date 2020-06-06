#include "./satisfiability.h"
#include <stdlib.h>
#include "../data_structures/hash_table.h"

ResultCode Sat_EvalClause(clause* clause, bool a, bool b, bool* satified) {
  if (clause == NULL || satified == NULL) return NullParameter;

  bool _a = clause->a > 0 ? true : false;
  if (!a) _a = !_a;

  bool _b = clause->b > 0 ? true : false;
  if (!b) _b = !_b;

  *satified = _a || _b;
  return Success;
}

ResultCode Sat_Solve(size_t n, clause clauses[n], bool* result) {
  if (clauses == NULL || result == NULL) return NullParameter;
  if (n == 0) return ArgumentOutOfRange;
  return Success;
}
