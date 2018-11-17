#include "include/TestHelpers.h"

extern int register_dijkstra_tests();

int register_tests() { return register_dijkstra_tests(); }

int main(void) { TestRunner(register_tests); }
