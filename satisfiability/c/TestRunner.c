#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int registerSatisfiabilityTests();

int run_tests() { return registerSatisfiabilityTests(); }

int main(void) { return TestRunner(run_tests); }
