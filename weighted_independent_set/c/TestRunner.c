#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int registerWeightedIndependentSetTests();

int run_tests() { return registerWeightedIndependentSetTests(); }

int main(void) { return TestRunner(run_tests); }
