#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int register_min_spanning_tree_tests();

int run_tests() { return register_min_spanning_tree_tests(); }

int main(void) { return TestRunner(run_tests); }
