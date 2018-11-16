#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "include/TestHelpers.h"

extern int quick_sort_suite();
extern int partition_suite();
extern int bubble_sort_suite();
extern int insertion_sort_suite();
extern int selection_sort_suite();
extern int merge_sort_test_suite();

int register_tests()
{
    if (bubble_sort_suite() != 0 || insertion_sort_suite() != 0
        || merge_sort_test_suite() != 0 || quick_sort_suite() != 0
        || selection_sort_suite() != 0 || partition_suite() != 0) {
        CU_cleanup_registry();
        return -1;
    }

    return 0;
}

int main(void) { return TestRunner(register_tests); }
