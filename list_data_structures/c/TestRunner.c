#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int register_linked_list_tests();
int register_array_tests();
int register_algo_timer_tests();
int register_sorted_array_tests();
int register_search_tree_tests();

int run_tests()
{
    return (register_array_tests() != 0 + register_algo_timer_tests()
               != 0 + register_sorted_array_tests()
               != 0 + register_search_tree_tests()
               != 0 + register_linked_list_tests() != 0)
        * -1;
}

int main(void) { return TestRunner(run_tests); }
