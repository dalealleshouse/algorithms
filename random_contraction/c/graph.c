#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "graph.h"

const size_t INITIAL_ALLOC = 100;
const size_t REALLOC_FACTOR = 2;
const static size_t BUFFER_SIZE = 1024;
const static char* TOKEN_DELIM = "\t";

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

graph* graph_init() { return _malloc(sizeof(graph)); }

static size_t calc_next_alloc_size(unsigned id)
{
    if (id < INITIAL_ALLOC)
        return INITIAL_ALLOC;

    return id * REALLOC_FACTOR;
}

int graph_add_vertex(graph* graph, const unsigned id)
{
    if (graph == NULL)
        return -1;

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

    graph->n++;
    graph->V[id].vertex_id = id;
    graph->V[id].degree = 0;
    graph->V[id].initalized = true;
    graph->V[id].consumed_size = 0;
    graph->V[id].consumed = NULL;

    return 0;
}

int graph_add_edge(graph* graph, const unsigned tail, const unsigned head)
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

static void shift_edges(edge* edges, size_t size, size_t edge_index)
{
    // size = 4
    // if index = 0 shift = 4 - 0 - 1 = 3
    // if index = 1 shift = 4 - 1 - 1 = 2
    // if index = 2 shift = 4 - 2 - 1 = 1
    // if index = 3 shift = 4 - 3 - 1 = 0
    // [0][1][2][3]
    memmove(&edges[edge_index], &edges[edge_index + 1],
        sizeof(edge) * (size - edge_index - 1));
}

static void delete_edge(graph* graph, const size_t edge_index)
{
    graph->V[graph->E[edge_index].head].degree--;
    graph->V[graph->E[edge_index].tail].degree--;
    shift_edges(graph->E, graph->m, edge_index);
    graph->m--;
}

static void delete_vertex(graph* graph, const unsigned id)
{
    graph->V[id].initalized = false;
    graph->n--;
}

static int make_super_vertex(vertex* ver, const vertex* doomed)
{
    size_t old_size = ver->consumed_size;
    ver->consumed_size += doomed->consumed_size + 1;

    if (ver->consumed == NULL)
        ver->consumed = _malloc(sizeof(unsigned) * ver->consumed_size);
    else
        ver->consumed
            = _realloc(ver->consumed, sizeof(unsigned) * ver->consumed_size);

    if (ver->consumed == NULL)
        return -1;

    ver->consumed[old_size] = doomed->vertex_id;

    if (doomed->consumed != NULL)
        memcpy(&ver->consumed[old_size + 1], doomed->consumed,
            sizeof(unsigned) * doomed->consumed_size);

    return 0;
}

int graph_collapse_edge(graph* graph, size_t edge_index)
{
    if (graph == NULL || edge_index >= graph->m)
        return -1;

    edge* edge = &graph->E[edge_index];
    vertex* tail = &graph->V[edge->tail];
    vertex* head = &graph->V[edge->head];

    delete_vertex(graph, edge->tail);
    make_super_vertex(head, tail);
    delete_edge(graph, edge_index);

    for (size_t i = 0; i < graph->m; i++) {
        if (graph->E[i].tail == tail->vertex_id) {
            graph->E[i].tail = head->vertex_id;
            head->degree++;
        }

        if (graph->E[i].head == tail->vertex_id) {
            graph->E[i].head = head->vertex_id;
            head->degree++;
        }

        if (graph->E[i].head == graph->E[i].tail) {
            delete_edge(graph, i);
            i -= 1;
        }
    }

    return 0;
}

static void graph_add_vertex_if_missing(graph* graph, unsigned vertex)
{
    if (vertex > graph->n_allocated || !graph->V[vertex].initalized)
        graph_add_vertex(graph, vertex);
}

graph* graph_read_from_file(const char* path)
{
    if (path == NULL)
        return NULL;

    if (access(path, R_OK) != 0) {
        printf("you no access man");
        return NULL;
    }

    FILE* file = fopen(path, "r");
    if (file == NULL)
        return NULL;

    graph* graph = graph_init();
    if (graph == NULL)
        return NULL;

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, file)) {
        char* remaining;
        unsigned vertex = strtoul(line, &remaining, 10);

        if (vertex == 0)
            continue;

        graph_add_vertex_if_missing(graph, vertex);

        unsigned edge = strtoul(remaining, &remaining, 10);
        while (edge > 0) {
            graph_add_vertex_if_missing(graph, edge);

            graph_add_edge(graph, vertex, edge);
            edge = strtoul(remaining, &remaining, 10);
        }
    }

    fclose(file);

    return graph;
}

void graph_destroy(graph* graph)
{
    if (graph == NULL)
        return;

    if (graph->V != NULL)
        for (size_t i = 0; i < graph->n_allocated; i++)
            free(graph->V[i].consumed);

    free(graph->V);
    free(graph->E);
    free(graph);
}

void graph_print(const graph* graph)
{
    for (size_t i = 0; i < graph->n_allocated; i++) {
        if (!graph->V[i].initalized)
            continue;

        vertex* v = &graph->V[i];

        if (v->consumed_size > 0) {
            printf("Super Vertex = %u (", v->vertex_id);

            for (size_t j = 0; j < v->consumed_size; j++)
                printf("%u-", v->consumed[j]);

            printf("), degree=%zu\n", v->degree);
        } else {
            printf("Vertex %u, degree=%zu\n", v->vertex_id, v->degree);
        }
    }

    for (size_t i = 0; i < graph->m; i++) {
        printf(
            "%zu = tail %u, head=%u\n", i, graph->E[i].tail, graph->E[i].head);
    }
}

graph* graph_clone(const graph* source)
{
    if (source == NULL)
        return NULL;

    graph* clone = graph_init();
    if (clone == NULL)
        return NULL;

    clone->m = source->m;
    clone->n = source->n;
    clone->m_allocated = source->m_allocated;
    clone->n_allocated = source->n_allocated;

    if (source->V != NULL) {
        size_t v_size = sizeof(vertex) * source->n_allocated;
        clone->V = _malloc(v_size);

        if (clone->V != NULL)
            memcpy(clone->V, source->V, v_size);
    }

    if (source->E != NULL) {
        size_t e_size = sizeof(edge) * source->m_allocated;
        clone->E = _malloc(e_size);

        if (clone->E != NULL)
            memcpy(clone->E, source->E, e_size);
    }

    return clone;
}
