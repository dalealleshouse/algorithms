#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int register_graph_search_tests();

int main(void) { return TestRunner(register_graph_search_tests); }
