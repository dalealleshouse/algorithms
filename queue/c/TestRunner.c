#include "include/TestHelpers.h"

extern int register_queue_tests();
extern int register_priority_queue_tests();

int register_tests()
{
    return register_queue_tests() + register_priority_queue_tests();
}

int main(void) { TestRunner(register_tests); }
