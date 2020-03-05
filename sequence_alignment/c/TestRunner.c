#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int registerSequenceAlignmentTests();

int run_tests() { return registerSequenceAlignmentTests(); }

int main(void) { return TestRunner(run_tests); }
