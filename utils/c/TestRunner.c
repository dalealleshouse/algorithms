#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "TestHelpers.h"

int registerGraphTests();
int registerOverflowTests();
int registerMathTests();

int register_tests() {
  return (registerGraphTests() != 0 + registerOverflowTests() !=
          0 + registerMathTests() != 0) *
         -1;
}

int main(void) { TestRunner(register_tests); }
