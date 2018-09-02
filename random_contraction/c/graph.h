#pragma once

#include <stddef.h>
#include <stdbool.h>

// Initial size of vertex and edge allocation 
const size_t INITIAL_ALLOC;

// Factor by when to increase allocation size when more space is required 
const size_t REALLOC_FACTOR;

typedef void* (*allocator)(const size_t size);
typedef void* (*reallocator)(void* ptr, const size_t size);

typedef struct
{
    // Id of the vertex
    unsigned vertex_id;

    // Number of edges connected to the vertex
    size_t degree;

    // Indicates if the vertex is initialized and valid
    bool initalized;
} vertex;

typedef struct
{
    // Id of the vertex at the tail of the edge
    unsigned tail;

    // Id of the vertex at the head of the edge
    unsigned head;
} edge;

typedef struct
{
    // Number of vertices
    size_t n;

    // The number of vertices that can be accommodated before reallocation.
    // This starts at 100 and doubles on every time more space is required
    size_t n_allocated;

    // Vertices
    vertex* V;

    // Number of edges
    size_t m;

    // Edges
    edge* E;

    // The number of edges that can be accommodated before reallocation.
    // This starts at 100 and doubles on every time more space is required
    size_t m_allocated;
} graph;

/*
 * Allows swapping out malloc with a different allocator
 * This was mainly used for testing
 */
void set_allocator(allocator allocator);
void* zero_allocator(const size_t size);

/*
 * Allows swapping out realloc with a different allocator
 * This was mainly used for testing
 */
void set_reallocator(reallocator reallocator);
void* standard_realloc(void* prt, const size_t size);

graph* graph_init();
int add_vertex(graph* graph, const unsigned id);
int add_edge(graph* graph, const unsigned tail, const unsigned head);
void graph_destroy(graph* graph);


graph* read_graph(const char* path);

