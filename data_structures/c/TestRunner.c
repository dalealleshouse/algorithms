#include "include/TestHelpers.h"

extern int register_queue_tests();
extern int register_priority_queue_tests();
extern int register_stack_tests();
extern int register_heap_tests();

int register_tests()
{
    return register_stack_tests() + register_queue_tests()
        + register_priority_queue_tests() + register_heap_tests();
}

int main(void) { TestRunner(register_tests); }
