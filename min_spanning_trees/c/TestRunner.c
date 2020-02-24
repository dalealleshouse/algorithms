#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

int register_min_spanning_tree_tests();
int register_kruskal_cluster_tests();

int run_tests()
{
    return (register_min_spanning_tree_tests()
               != 0 + register_kruskal_cluster_tests() != 0)
        * -1;
}

int main(void) { return TestRunner(run_tests); }
