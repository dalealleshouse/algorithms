#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int registerKnapsackProblemTests();

int run_tests() { return registerKnapsackProblemTests(); }

int main(void) { return TestRunner(run_tests); }
