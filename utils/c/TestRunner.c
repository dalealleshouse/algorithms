#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "TestHelpers.h"

int register_graph_tests();
int register_overflow_tests();

int register_tests()
{
    return (register_graph_tests() != 0 + register_overflow_tests() != 0) * -1;
}

int main(void) { TestRunner(register_tests); }
