#include <stdbool.h>
#include <stdlib.h>

#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

#include "graph.h"

#define CU_TEST_INFO(test_func)                                                \
    {                                                                          \
#test_func, test_func                                                  \
    }

static int noop(void) { return 0; }

static bool bad_malloc_run = false;
static void* bad_malloc(size_t size)
{
    (void)size;
    bad_malloc_run = true;
    return NULL;
}
static void intercept_malloc()
{
    bad_malloc_run = false;
    set_allocator(bad_malloc);
}
static void malloc_intercepted()
{
    CU_ASSERT_EQUAL(true, bad_malloc_run);
    bad_malloc_run = false;
    set_allocator(zero_allocator);
}

static bool bad_realloc_run = false;
static void* bad_realloc(void* ptr, const size_t size)
{
    (void)size;

    bad_realloc_run = true;
    free(ptr);
    return NULL;
}
static void intercept_realloc()
{
    bad_realloc_run = false;
    set_reallocator(bad_realloc);
}
static void realloc_intercepted()
{
    CU_ASSERT_EQUAL(true, bad_realloc_run);
    bad_realloc_run = false;
    set_reallocator(standard_realloc);
}

/*************************** graph_init ***************************************/
static void graph_init_bad_malloc()
{
    intercept_malloc();

    graph* graph = graph_init();
    CU_ASSERT_PTR_NULL(graph);
    malloc_intercepted();

    graph_destroy(graph);
}

static void graph_init_intializes_graph()
{
    graph* graph = graph_init();

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_PTR_NULL(graph->E);
    CU_ASSERT_PTR_NULL(graph->V);
    CU_ASSERT_EQUAL(0, graph->n);
    CU_ASSERT_EQUAL(0, graph->m);
    CU_ASSERT_EQUAL(0, graph->n_allocated);

    graph_destroy(graph);
}

/*************************** graph_add_vertex
 * ***************************************/
static void graph_add_vertex_null()
{
    int result = graph_add_vertex(NULL, 0);
    CU_ASSERT_EQUAL(-1, result);
}

static void graph_add_vertex_bad_malloc()
{
    graph* graph = graph_init();
    intercept_malloc();
    int result = graph_add_vertex(graph, 1);

    CU_ASSERT_EQUAL(-1, result);
    CU_ASSERT_EQUAL(0, graph->n);
    malloc_intercepted();

    graph_destroy(graph);
}

static void graph_add_vertex_first()
{
    graph* graph = graph_init();
    int result = graph_add_vertex(graph, 1);

    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_EQUAL(1, graph->n);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->V);

    CU_ASSERT_EQUAL(1, graph->V[1].vertex_id);
    CU_ASSERT_EQUAL(0, graph->V[1].degree);
    CU_ASSERT_EQUAL(0, graph->V[1].consumed_size);
    CU_ASSERT_PTR_NULL(graph->V[1].consumed);

    graph_destroy(graph);
}

static void graph_add_vertex_adds_five()
{
    graph* graph = graph_init();

    for (size_t i = 0; i < 5; i++) {
        int result = graph_add_vertex(graph, i);
        CU_ASSERT_EQUAL(0, result);
    }

    CU_ASSERT_EQUAL(5, graph->n);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->V);

    for (size_t i = 0; i < 5; i++) {
        CU_ASSERT_EQUAL(i, graph->V[i].vertex_id);
        CU_ASSERT_EQUAL(0, graph->V[i].degree);
    }

    graph_destroy(graph);
}

static void graph_add_vertex_bad_realloc()
{
    graph* graph = graph_init();

    intercept_realloc();
    int result = graph_add_vertex(graph, 1);
    CU_ASSERT_EQUAL(0, result);

    result = graph_add_vertex(graph, INITIAL_ALLOC + 1);
    CU_ASSERT_EQUAL(-1, result);
    CU_ASSERT_EQUAL(1, graph->n);
    realloc_intercepted();

    graph_destroy(graph);
}

static void graph_add_vertex_pre_allocated()
{
    graph* graph = graph_init();
    graph_add_vertex(graph, 10);

    CU_ASSERT_EQUAL(10, graph->V[10].vertex_id);
    CU_ASSERT_EQUAL(true, graph->V[10].initalized);

    graph_add_vertex(graph, 5);
    CU_ASSERT_EQUAL(5, graph->V[5].vertex_id);
    CU_ASSERT_EQUAL(true, graph->V[5].initalized);

    graph_destroy(graph);
}

static void graph_add_vertex_intializes_empty_slots()
{
    graph* graph = graph_init();
    graph_add_vertex(graph, 10);

    CU_ASSERT_EQUAL(10, graph->V[10].vertex_id);
    CU_ASSERT_EQUAL(true, graph->V[10].initalized);

    for (size_t i = 0; i < graph->n_allocated; i++) {
        if (i == 10)
            continue;

        CU_ASSERT_EQUAL(false, graph->V[i].initalized);
    }

    graph_destroy(graph);
}

static void graph_add_vertex_duplicate()
{
    graph* graph = graph_init();
    graph_add_vertex(graph, 10);
    int result = graph_add_vertex(graph, 10);
    CU_ASSERT_EQUAL(-1, result);
    CU_ASSERT_EQUAL(1, graph->n);

    graph_destroy(graph);
}

/*************************** graph_add_edge ***********************************/
static void graph_add_edge_null()
{
    int result = graph_add_edge(NULL, 0, 0);
    CU_ASSERT_EQUAL(-1, result);
}

static void graph_add_edge_bad_malloc()
{
    graph* graph = graph_init();
    graph_add_vertex(graph, 1);
    graph_add_vertex(graph, 2);

    intercept_malloc();
    int result = graph_add_edge(graph, 1, 2);
    CU_ASSERT_EQUAL(-1, result);
    malloc_intercepted();

    graph_destroy(graph);
}

static void graph_add_edge_first()
{
    graph* graph = graph_init();
    graph_add_vertex(graph, 1);
    graph_add_vertex(graph, 2);

    int result = graph_add_edge(graph, 1, 2);
    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_EQUAL(1, graph->m);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->E);
    CU_ASSERT_EQUAL(1, graph->E->tail);
    CU_ASSERT_EQUAL(2, graph->E->head);
    CU_ASSERT_EQUAL(1, graph->V[1].degree);
    CU_ASSERT_EQUAL(1, graph->V[2].degree);

    graph_destroy(graph);
}

static void graph_add_edge_adds_five()
{
    graph* graph = graph_init();

    for (size_t i = 0; i < 6; i++)
        graph_add_vertex(graph, i);

    for (size_t i = 0; i < 5; i++) {
        int result = graph_add_edge(graph, i, i + 1);
        CU_ASSERT_EQUAL(0, result);
    }

    CU_ASSERT_EQUAL(5, graph->m);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->E);

    for (size_t i = 0; i < 5; i++) {
        CU_ASSERT_EQUAL(i, graph->E[i].tail);
        CU_ASSERT_EQUAL(i + 1, graph->E[i].head);
    }

    for (size_t i = 0; i < 6; i++) {
        if (i == 0 || i == 5) {
            CU_ASSERT_EQUAL(1, graph->V[i].degree);
        } else
            CU_ASSERT_EQUAL(2, graph->V[i].degree);
    }

    graph_destroy(graph);
}

static void graph_add_edge_bad_realloc()
{
    graph* graph = graph_init();

    for (size_t i = 0; i < INITIAL_ALLOC + 1; i++)
        graph_add_vertex(graph, i);

    for (size_t i = 0; i < INITIAL_ALLOC; i++)
        graph_add_edge(graph, i, i + 1);

    intercept_realloc();
    int result = graph_add_edge(graph, 2, 1);
    CU_ASSERT_EQUAL(-1, result);
    realloc_intercepted();

    graph_destroy(graph);
}

static void graph_add_edge_updates_vertex()
{
    graph* graph = graph_init();
    graph_add_vertex(graph, 1);
    graph_add_vertex(graph, 2);
    graph_add_vertex(graph, 3);

    graph_add_edge(graph, 1, 2);
    graph_add_edge(graph, 2, 3);
    graph_add_edge(graph, 2, 1);

    CU_ASSERT_EQUAL(2, graph->V[1].degree);
    CU_ASSERT_EQUAL(3, graph->V[2].degree);
    CU_ASSERT_EQUAL(1, graph->V[3].degree);

    graph_destroy(graph);
}

static void graph_add_edge_self_loop()
{
    graph* graph = graph_init();
    graph_add_vertex(graph, 1);

    int result = graph_add_edge(graph, 1, 1);
    CU_ASSERT_EQUAL(-1, result);

    graph_destroy(graph);
}

static void graph_add_edge_invalid_vertex()
{
    graph* graph = graph_init();

    int result = graph_add_edge(graph, 1, 10);
    CU_ASSERT_EQUAL(-1, result);

    graph_destroy(graph);
}

/*************************** graph_read_from_file *****************************/
static void graph_read_from_file_null()
{
    graph* graph = graph_read_from_file(NULL);
    CU_ASSERT_PTR_NULL(graph);
    graph_destroy(graph);
}

static void graph_read_from_file_bad_path()
{
    graph* graph = graph_read_from_file("some_bogus_path.txt");
    CU_ASSERT_PTR_NULL(graph);
    graph_destroy(graph);
}

static void graph_read_from_file_small()
{
    graph* graph = graph_read_from_file("src/SmallGraph.txt");

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(5, graph->n);
    CU_ASSERT_EQUAL(8, graph->m);

    CU_ASSERT_EQUAL(2, graph->V[1].degree);
    CU_ASSERT_EQUAL(4, graph->V[2].degree);
    CU_ASSERT_EQUAL(3, graph->V[3].degree);
    CU_ASSERT_EQUAL(4, graph->V[4].degree);
    CU_ASSERT_EQUAL(3, graph->V[5].degree);

    graph_destroy(graph);
}

static void graph_read_from_file_medium()
{
    graph* graph = graph_read_from_file("src/GraphMedium.txt");

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(8, graph->n);
    CU_ASSERT_EQUAL(14, graph->m);

    graph_destroy(graph);
}

static void graph_read_from_file_standard()
{
    graph* graph = graph_read_from_file("src/Graph.txt");

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(200, graph->n);

    graph_destroy(graph);
}

static void graph_read_invalid()
{
    graph* graph = graph_read_from_file("src/BadGraph.txt");

    CU_ASSERT_PTR_NOT_NULL_FATAL(graph);
    CU_ASSERT_EQUAL(1, graph->n);
    CU_ASSERT_EQUAL(0, graph->m);

    graph_destroy(graph);
}

/*************************** graph_collapse_edge ******************************/
static void graph_collapse_edge_null()
{
    int result = graph_collapse_edge(NULL, 0);
    CU_ASSERT_EQUAL(-1, result);
}

static void graph_collapse_edge_invalid_index()
{
    graph* graph = graph_read_from_file("src/SmallGraph.txt");

    int result = graph_collapse_edge(graph, graph->m);
    CU_ASSERT_EQUAL(-1, result);

    graph_destroy(graph);
}

static void graph_collapse_edge_tail()
{
    graph* graph = graph_read_from_file("src/SmallGraph.txt");
    size_t m = graph->m;
    size_t n = graph->n;

    int result = graph_collapse_edge(graph, 1);

    CU_ASSERT_EQUAL(0, result);

    CU_ASSERT_EQUAL(n - 1, graph->n);
    CU_ASSERT_EQUAL(false, graph->V[1].initalized);
    CU_ASSERT_EQUAL(true, graph->V[4].initalized);

    CU_ASSERT_EQUAL(4, graph->V[2].degree);
    CU_ASSERT_EQUAL(3, graph->V[3].degree);
    CU_ASSERT_EQUAL(4, graph->V[4].degree);
    CU_ASSERT_EQUAL(3, graph->V[5].degree);

    CU_ASSERT_EQUAL(m - 1, graph->m);

    graph_destroy(graph);
}

static void graph_collapse_edge_head()
{
    graph* graph = graph_read_from_file("src/SmallGraph.txt");
    size_t m = graph->m;
    size_t n = graph->n;

    int result = graph_collapse_edge(graph, 7);
    CU_ASSERT_EQUAL(0, result);

    CU_ASSERT_EQUAL(n - 1, graph->n);
    CU_ASSERT_EQUAL(false, graph->V[4].initalized);
    CU_ASSERT_EQUAL(true, graph->V[5].initalized);

    CU_ASSERT_EQUAL(2, graph->V[1].degree);
    CU_ASSERT_EQUAL(4, graph->V[2].degree);
    CU_ASSERT_EQUAL(3, graph->V[3].degree);
    CU_ASSERT_EQUAL(5, graph->V[5].degree);

    CU_ASSERT_EQUAL(m - 1, graph->m);

    graph_destroy(graph);
}

static void graph_collapse_edge_self_loop()
{
    graph* graph = graph_read_from_file("src/SmallGraph.txt");
    size_t m = graph->m;
    size_t n = graph->n;

    // tail = 4, head = 5
    int result = graph_collapse_edge(graph, 7);
    CU_ASSERT_EQUAL(0, result);

    // tail = 2, head = 5
    result = graph_collapse_edge(graph, 4);
    CU_ASSERT_EQUAL(0, result);

    CU_ASSERT_EQUAL(n - 2, graph->n);

    CU_ASSERT_EQUAL(2, graph->V[1].degree);
    CU_ASSERT_EQUAL(3, graph->V[3].degree);
    CU_ASSERT_EQUAL(5, graph->V[5].degree);

    CU_ASSERT_EQUAL(m - 3, graph->m);

    graph_destroy(graph);
}

static void graph_collapse_edge_three_consecutive()
{
    graph* graph = graph_read_from_file("src/SmallGraph.txt");

    graph_collapse_edge(graph, 1);
    graph_collapse_edge(graph, 4);
    graph_collapse_edge(graph, 2);

    CU_ASSERT_EQUAL(2, graph->n);
    CU_ASSERT_EQUAL(3, graph->m);

    graph_destroy(graph);
}

static void graph_collapse_edge_large()
{
    graph* graph = graph_read_from_file("src/Graph.txt");
    size_t m = graph->m;
    size_t n = graph->n;

    int result = graph_collapse_edge(graph, 4180);
    CU_ASSERT_EQUAL(0, result);

    CU_ASSERT_EQUAL(n - 1, graph->n);
    CU_ASSERT_EQUAL(m - 2, graph->m);

    graph_destroy(graph);
}

static void graph_collapse_edge_creates_super_vertex()
{
    graph* graph = graph_read_from_file("src/SmallGraph.txt");

    graph_collapse_edge(graph, 1);

    CU_ASSERT_EQUAL(1, graph->V[4].consumed_size);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->V[4].consumed);
    CU_ASSERT_EQUAL(1, graph->V[4].consumed[0]);

    graph_destroy(graph);
}

static void graph_collapse_edge_updates_super_vertex()
{
    graph* graph = graph_read_from_file("src/SmallGraph.txt");

    graph_collapse_edge(graph, 1);
    graph_collapse_edge(graph, 2);
    graph_collapse_edge(graph, 4);

    CU_ASSERT_EQUAL(3, graph->V[5].consumed_size);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->V[5].consumed);
    CU_ASSERT_EQUAL(4, graph->V[5].consumed[0]);
    CU_ASSERT_EQUAL(1, graph->V[5].consumed[1]);
    CU_ASSERT_EQUAL(2, graph->V[5].consumed[2]);

    graph_destroy(graph);
}

/*************************** graph_clone **************************************/
static void graphs_are_identical(graph* x, graph* y)
{
    CU_ASSERT_PTR_NOT_NULL_FATAL(x);
    CU_ASSERT_PTR_NOT_NULL_FATAL(y);

    CU_ASSERT_PTR_NOT_EQUAL(x, y);

    CU_ASSERT_EQUAL(x->n, y->n);
    CU_ASSERT_EQUAL(x->m, y->m);
    CU_ASSERT_EQUAL(x->n_allocated, y->n_allocated);
    CU_ASSERT_EQUAL(x->m_allocated, y->m_allocated);

    if (x->V == NULL) {
        CU_ASSERT_PTR_NULL(y->V);
    } else {
        CU_ASSERT_PTR_NOT_NULL_FATAL(y->V);
        CU_ASSERT_PTR_NOT_EQUAL(x->V, y->V);
        CU_ASSERT_EQUAL(0, memcmp(x->V, y->V, sizeof(vertex) * x->n_allocated));
    }

    if (x->E == NULL) {
        CU_ASSERT_PTR_NULL(y->E);
    } else {
        CU_ASSERT_PTR_NOT_NULL_FATAL(y->E);
        CU_ASSERT_PTR_NOT_EQUAL(x->E, y->E);
        CU_ASSERT_EQUAL(0, memcmp(x->E, y->E, sizeof(edge) * x->m_allocated));
    }
}

static void graph_clone_null()
{
    graph* result = graph_clone(NULL);
    CU_ASSERT_PTR_NULL(result);
}

static void graph_clone_standard()
{
    graph* source = graph_read_from_file("src/SmallGraph.txt");

    graph* clone = graph_clone(source);
    graphs_are_identical(source, clone);

    graph_destroy(source);
    graph_destroy(clone);
}

static void graph_clone_empty()
{
    graph* source = graph_init();
    graph* clone = graph_clone(source);

    graphs_are_identical(source, clone);

    graph_destroy(source);
    graph_destroy(clone);
}

static void graph_clone_bad_malloc()
{
    graph* source = graph_init();

    intercept_malloc();
    graph* clone = graph_clone(source);

    CU_ASSERT_PTR_NULL(clone);
    malloc_intercepted();

    graph_destroy(source);
}

static void graph_clone_large()
{
    graph* source = graph_read_from_file("src/Graph.txt");

    graph* clone = graph_clone(source);
    graphs_are_identical(source, clone);

    graph_destroy(source);
    graph_destroy(clone);
}

int register_graph_suites()
{
    CU_TestInfo graph_init_tests[] = { CU_TEST_INFO(graph_init_bad_malloc),
        CU_TEST_INFO(graph_init_intializes_graph), CU_TEST_INFO_NULL };

    CU_TestInfo graph_add_vertex_tests[]
        = { CU_TEST_INFO(graph_add_vertex_null),
              CU_TEST_INFO(graph_add_vertex_bad_malloc),
              CU_TEST_INFO(graph_add_vertex_first),
              CU_TEST_INFO(graph_add_vertex_adds_five),
              CU_TEST_INFO(graph_add_vertex_bad_realloc),
              CU_TEST_INFO(graph_add_vertex_intializes_empty_slots),
              CU_TEST_INFO(graph_add_vertex_pre_allocated),
              CU_TEST_INFO(graph_add_vertex_duplicate), CU_TEST_INFO_NULL };

    CU_TestInfo graph_add_edge_tests[] = { CU_TEST_INFO(graph_add_edge_null),
        CU_TEST_INFO(graph_add_edge_bad_malloc),
        CU_TEST_INFO(graph_add_edge_first),
        CU_TEST_INFO(graph_add_edge_adds_five),
        CU_TEST_INFO(graph_add_edge_bad_realloc),
        CU_TEST_INFO(graph_add_edge_updates_vertex),
        CU_TEST_INFO(graph_add_edge_self_loop),
        CU_TEST_INFO(graph_add_edge_invalid_vertex), CU_TEST_INFO_NULL };

    CU_TestInfo graph_read_from_file_tests[]
        = { CU_TEST_INFO(graph_read_from_file_null),
              CU_TEST_INFO(graph_read_from_file_bad_path),
              CU_TEST_INFO(graph_read_from_file_small),
              CU_TEST_INFO(graph_read_from_file_standard),
              CU_TEST_INFO(graph_read_invalid),
              CU_TEST_INFO(graph_read_from_file_medium), CU_TEST_INFO_NULL };

    CU_TestInfo graph_collapse_edge_tests[]
        = { CU_TEST_INFO(graph_collapse_edge_null),
              CU_TEST_INFO(graph_collapse_edge_invalid_index),
              CU_TEST_INFO(graph_collapse_edge_tail),
              CU_TEST_INFO(graph_collapse_edge_head),
              CU_TEST_INFO(graph_collapse_edge_self_loop),
              CU_TEST_INFO(graph_collapse_edge_three_consecutive),
              CU_TEST_INFO(graph_collapse_edge_large),
              CU_TEST_INFO(graph_collapse_edge_creates_super_vertex),
              CU_TEST_INFO(graph_collapse_edge_updates_super_vertex),
              CU_TEST_INFO_NULL };

    CU_TestInfo graph_clone_tests[] = { CU_TEST_INFO(graph_clone_null),
        CU_TEST_INFO(graph_clone_standard), CU_TEST_INFO(graph_clone_empty),
        CU_TEST_INFO(graph_clone_bad_malloc), CU_TEST_INFO(graph_clone_large),
        CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "graph_init suite",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = graph_init_tests },
        { .pName = "graph_add_vertex suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = graph_add_vertex_tests },
        { .pName = "graph_add_edge suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = graph_add_edge_tests },
        { .pName = "graph_read_from_file suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = graph_read_from_file_tests },
        { .pName = "graph_collapse_edge suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = graph_collapse_edge_tests },
        { .pName = "graph_clone suite",
            .pInitFunc = noop,
            .pCleanupFunc = noop,
            .pTests = graph_clone_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
