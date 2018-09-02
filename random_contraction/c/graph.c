#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "graph.h"

const size_t INITIAL_ALLOC = 100;
const size_t REALLOC_FACTOR = 2;

static allocator _malloc = zero_allocator;
static reallocator _realloc = standard_realloc;

void set_allocator(allocator allocator) { _malloc = allocator; }
void set_reallocator(reallocator reallocator) { _realloc = reallocator; }

void* zero_allocator(const size_t size) { return calloc(size, 1); }

void* standard_realloc(void* ptr, const size_t size)
{
    // we should be checking for arithmetic overflow, but it may be overkill for
    // this application...
    return realloc(ptr, size);
}

graph* graph_init()
{
    graph* g = _malloc(sizeof(graph));

    return g;
}

static size_t calc_next_alloc_size(unsigned id)
{
    if (id < INITIAL_ALLOC)
        return INITIAL_ALLOC;

    return id * REALLOC_FACTOR;
}

int add_vertex(graph* graph, const unsigned id)
{
    if (graph == NULL)
        return -1;

    graph->n++;

    if (graph->n_allocated <= id) {
        size_t next_size = calc_next_alloc_size(id);
        if (graph->V == NULL)
            graph->V = _malloc(sizeof(vertex) * next_size);
        else
            graph->V = _realloc(graph->V, sizeof(vertex) * next_size);

        if (graph->V == NULL)
            return -1;

        memset(&graph->V[graph->n_allocated], 0,
            sizeof(graph->V[0]) * (next_size - graph->n_allocated));

        graph->n_allocated = next_size;
    }

    if (graph->V[id].initalized == true)
        return -1;

    graph->V[id].vertex_id = id;
    graph->V[id].degree = 0;
    graph->V[id].initalized = true;

    return 0;
}

int add_edge(graph* graph, const unsigned tail, const unsigned head)
{
    if (graph == NULL || tail == head)
        return -1;

    if (tail >= graph->n_allocated || head >= graph->n_allocated
        || graph->V[tail].initalized == false
        || graph->V[head].initalized == false)
        return -1;

    graph->m++;

    if (graph->m_allocated < graph->m) {
        size_t next_size = calc_next_alloc_size(graph->m);
        if (graph->E == NULL)
            graph->E = _malloc(sizeof(edge) * next_size);
        else
            graph->E = _realloc(graph->E, sizeof(edge) * next_size);

        graph->m_allocated = next_size;
    }

    if (graph->E == NULL)
        return -1;

    edge* e = &graph->E[graph->m - 1];

    e->tail = tail;
    e->head = head;
    graph->V[tail].degree++;
    graph->V[head].degree++;

    return 0;
}

void graph_destroy(graph* graph)
{
    if (graph == NULL)
        return;

    free(graph->V);
    free(graph->E);
    free(graph);
}
