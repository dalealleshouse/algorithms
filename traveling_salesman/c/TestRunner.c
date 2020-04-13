#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int registerTravelingSalesmanTests();

int run_tests() { return registerTravelingSalesmanTests(); }

int main(void) { return TestRunner(run_tests); }
