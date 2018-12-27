#include "include/TestHelpers.h"

extern int register_queue_tests();
extern int register_priority_queue_tests();
extern int register_stack_tests();
extern int register_heap_tests();
extern int register_hash_table_tests();

int register_tests()
{
    return (register_stack_tests() != 0 + register_queue_tests()
               != 0 + register_priority_queue_tests()
               != 0 + register_heap_tests() != 0 + register_hash_table_tests()
               != 0)
        * -1;
}

int main(void) { TestRunner(register_tests); }
