#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int register_running_median_tests();

int run_tests() { return register_running_median_tests(); }

int main(void) { return TestRunner(run_tests); }
