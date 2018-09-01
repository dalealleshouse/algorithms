#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "random_contraction.h"

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int noop(void) { return 0; }

/*************************** graph ********************************************/

// Create graph
// Insert vertex
// Insert edge
// delete vertex
// delete edge

/*************************** min_cut ******************************************/
static void min_cut_null()
{
    int result = min_cut(NULL);
    CU_ASSERT_EQUAL(-1, result);
}

static void init_initalizes_values()
{
    /* list list; */
    /* list_item dummy; */

    /* // put some values in just make sure the test isn't passing just b/c the
     */
    /* // list just happens to be all 0 on creation */
    /* list.size = 5; */
    /* list.head = &dummy; */
    /* list.tail = &dummy; */

    /* int result = list_init(&list); */

    /* CU_ASSERT_EQUAL(0, result); */
    /* CU_ASSERT_EQUAL(0, list.size); */
    /* CU_ASSERT_PTR_NULL(list.head); */
    /* CU_ASSERT_PTR_NULL(list.tail); */
}

/*************************** collapse_edge ************************************/
static void collapse_edge_null()
{
    int result = collapse_edge(NULL, 0);
    CU_ASSERT_EQUAL(-1, result);
}

/* static void collaspe_edge_two_vertices() */
/* { */
/*     vertex vertices[] = { { 0, 0, NULL }, { 1, 0, NULL } }; */
/*     edge edges[] = { { &vertices[0], &vertices[1] } }; */
/*     vertices[0].edges = &edges[0]; */
/*     vertices[0].edge_count = 1; */

/*     graph graph = { &vertices[0], 2, &edges[0], 1 }; */
/*     (void)graph; */
/* } */

// Pick and edge at random
// fuse the two ends into a single vertex
// keep parallel edges
// delete self-referencing edges

int register_min_cut_test_suites()
{
    CU_TestInfo min_cut_tests[] = { CU_TEST_INFO(min_cut_null),
        CU_TEST_INFO(init_initalizes_values), CU_TEST_INFO_NULL };

    CU_TestInfo collapse_edge_tests[]
        = { CU_TEST_INFO(collapse_edge_null), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "min_cut suite",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = min_cut_tests },
        { .pName = "collapse_edge suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = collapse_edge_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
