#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int registerHuffmanCodeTests();
int registerBitArrayTests();

int run_tests() { return registerHuffmanCodeTests(); }
int main(void) { return TestRunner(run_tests); }
