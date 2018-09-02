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

/*************************** graph_init **************************************/
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

/*************************** add_vertex **************************************/
static void add_vertex_null()
{
    int result = add_vertex(NULL, 0);
    CU_ASSERT_EQUAL(-1, result);
}

static void add_vertex_bad_malloc()
{
    intercept_malloc();
    graph* graph = graph_init();
    int result = add_vertex(graph, 1);

    CU_ASSERT_EQUAL(-1, result);
    malloc_intercepted();

    graph_destroy(graph);
}

static void add_vertex_first()
{
    graph* graph = graph_init();
    int result = add_vertex(graph, 1);

    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_EQUAL(1, graph->n);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->V);

    CU_ASSERT_EQUAL(1, graph->V[1].vertex_id);
    CU_ASSERT_EQUAL(0, graph->V[1].degree);

    graph_destroy(graph);
}

static void add_vertex_adds_five()
{
    graph* graph = graph_init();

    for (size_t i = 0; i < 5; i++) {
        int result = add_vertex(graph, i);
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

static void add_vertex_bad_realloc()
{
    intercept_realloc();
    graph* graph = graph_init();

    int result = add_vertex(graph, 1);
    CU_ASSERT_EQUAL(0, result);

    result = add_vertex(graph, INITIAL_ALLOC + 1);
    CU_ASSERT_EQUAL(-1, result);
    realloc_intercepted();

    graph_destroy(graph);
}

static void add_vertex_pre_allocated()
{
    graph* graph = graph_init();
    add_vertex(graph, 10);

    CU_ASSERT_EQUAL(10, graph->V[10].vertex_id);
    CU_ASSERT_EQUAL(true, graph->V[10].initalized);

    add_vertex(graph, 5);
    CU_ASSERT_EQUAL(5, graph->V[5].vertex_id);
    CU_ASSERT_EQUAL(true, graph->V[5].initalized);

    graph_destroy(graph);
}

static void add_vertex_intializes_empty_slots()
{
    graph* graph = graph_init();
    add_vertex(graph, 10);

    CU_ASSERT_EQUAL(10, graph->V[10].vertex_id);
    CU_ASSERT_EQUAL(true, graph->V[10].initalized);

    for (size_t i = 0; i < graph->n_allocated; i++) {
        if (i == 10)
            continue;

        CU_ASSERT_EQUAL(false, graph->V[i].initalized);
    }

    graph_destroy(graph);
}

static void add_vertex_duplicate()
{
    graph* graph = graph_init();
    add_vertex(graph, 10);
    int result = add_vertex(graph, 10);
    CU_ASSERT_EQUAL(-1, result);

    graph_destroy(graph);
}

/*************************** add_edge ****************************************/
static void add_edge_null()
{
    int result = add_edge(NULL, 0, 0);
    CU_ASSERT_EQUAL(-1, result);
}

static void add_edge_bad_malloc()
{
    graph* graph = graph_init();
    add_vertex(graph, 1);
    add_vertex(graph, 2);

    intercept_malloc();
    int result = add_edge(graph, 1, 2);
    CU_ASSERT_EQUAL(-1, result);
    malloc_intercepted();

    graph_destroy(graph);
}

static void add_edge_first()
{
    graph* graph = graph_init();
    add_vertex(graph, 1);
    add_vertex(graph, 2);

    int result = add_edge(graph, 1, 2);
    CU_ASSERT_EQUAL(0, result);
    CU_ASSERT_EQUAL(1, graph->m);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->E);
    CU_ASSERT_EQUAL(1, graph->E->tail);
    CU_ASSERT_EQUAL(2, graph->E->head);

    graph_destroy(graph);
}

static void add_edge_adds_five()
{
    graph* graph = graph_init();

    for (size_t i = 0; i < 6; i++)
        add_vertex(graph, i);

    for (size_t i = 0; i < 5; i++) {
        int result = add_edge(graph, i, i + 1);
        CU_ASSERT_EQUAL(0, result);
    }

    CU_ASSERT_EQUAL(5, graph->m);
    CU_ASSERT_PTR_NOT_NULL_FATAL(graph->E);

    for (size_t i = 0; i < 5; i++) {
        CU_ASSERT_EQUAL(i, graph->E[i].tail);
        CU_ASSERT_EQUAL(i + 1, graph->E[i].head);
    }

    graph_destroy(graph);
}

static void add_edge_bad_realloc()
{
    graph* graph = graph_init();

    for (size_t i = 0; i < INITIAL_ALLOC + 1; i++)
        add_vertex(graph, i);

    for (size_t i = 0; i < INITIAL_ALLOC; i++)
        add_edge(graph, i, i + 1);

    intercept_realloc();
    int result = add_edge(graph, 2, 1);
    CU_ASSERT_EQUAL(-1, result);
    realloc_intercepted();

    graph_destroy(graph);
}

static void add_edge_updates_vertex()
{
    graph* graph = graph_init();
    add_vertex(graph, 1);
    add_vertex(graph, 2);
    add_vertex(graph, 3);

    add_edge(graph, 1, 2);
    add_edge(graph, 2, 3);
    add_edge(graph, 2, 1);

    CU_ASSERT_EQUAL(2, graph->V[1].degree);
    CU_ASSERT_EQUAL(3, graph->V[2].degree);
    CU_ASSERT_EQUAL(1, graph->V[3].degree);

    graph_destroy(graph);
}

static void add_edge_self_loop()
{
    graph* graph = graph_init();
    add_vertex(graph, 1);

    int result = add_edge(graph, 1, 1);
    CU_ASSERT_EQUAL(-1, result);

    graph_destroy(graph);
}

static void add_edge_invalid_vertex()
{
    graph* graph = graph_init();

    int result = add_edge(graph, 1, 10);
    CU_ASSERT_EQUAL(-1, result);

    graph_destroy(graph);
}

/* int remove_vertex(graph* graph); */
/* int remove_edge(graph* graph); */
/* int graph_destroy(graph* graph); */

int register_graph_suites()
{
    CU_TestInfo graph_tests[] = { CU_TEST_INFO(graph_init_bad_malloc),
        CU_TEST_INFO(graph_init_intializes_graph),
        CU_TEST_INFO(add_vertex_null), CU_TEST_INFO(add_vertex_bad_malloc),
        CU_TEST_INFO(add_vertex_first), CU_TEST_INFO(add_vertex_adds_five),
        CU_TEST_INFO(add_vertex_bad_realloc),
        CU_TEST_INFO(add_vertex_intializes_empty_slots),
        CU_TEST_INFO(add_vertex_pre_allocated),
        CU_TEST_INFO(add_vertex_duplicate), CU_TEST_INFO(add_edge_null),
        CU_TEST_INFO(add_edge_bad_malloc), CU_TEST_INFO(add_edge_first),
        CU_TEST_INFO(add_edge_adds_five), CU_TEST_INFO(add_edge_bad_realloc),
        CU_TEST_INFO(add_edge_updates_vertex), CU_TEST_INFO(add_edge_self_loop),
        CU_TEST_INFO(add_edge_invalid_vertex), CU_TEST_INFO_NULL };

    CU_SuiteInfo suites[] = { { .pName = "graph suite",
                                  .pInitFunc = noop,
                                  .pCleanupFunc = noop,
                                  .pTests = graph_tests },
        CU_SUITE_INFO_NULL };

    return CU_register_suites(suites);
}
