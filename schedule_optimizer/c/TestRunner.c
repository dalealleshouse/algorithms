#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int register_schedule_optimizer_tests();

int run_tests() { return register_schedule_optimizer_tests(); }

int main(void) { return TestRunner(run_tests); }
