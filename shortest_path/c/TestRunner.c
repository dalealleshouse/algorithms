#include "include/TestHelpers.h"

extern int register_dijkstra_tests();
extern int register_sp_tests();

int register_tests() { return register_sp_tests() + register_dijkstra_tests(); }

int main(void) { TestRunner(register_tests); }
