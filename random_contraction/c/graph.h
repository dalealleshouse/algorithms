#pragma once

#include <stdbool.h>
#include <stddef.h>

// Initial size of vertex and edge allocation
const size_t INITIAL_ALLOC;

// Factor by when to increase allocation size when more space is required
const size_t REALLOC_FACTOR;

typedef void* (*allocator)(const size_t size);
typedef void* (*reallocator)(void* ptr, const size_t size);

typedef enum {
    Graph_InvalidEdgeIndex = -6,
    Graph_InvalidVertex = -5,
    Graph_EdgeIsSelfLoop = -4,
    Graph_DuplicateVertex = -3,
    Graph_FailedMemoryAllocation = -2,
    Graph_NullParameter = -1,
    Graph_Success = 0
} GraphResult;

typedef struct {
    // Indicates if the vertex is initialized and valid
    bool initalized;

    // Id of the vertex
    unsigned vertex_id;

    // Number of edges connected to the vertex
    size_t degree;

    // The number of vertices that have been combined with this vertex
    size_t consumed_size;

    // All vertices that have been combined into this v
    unsigned* consumed;
} vertex;

typedef struct {
    // Id of the vertex at the tail of the edge
    unsigned tail;

    // Id of the vertex at the head of the edge
    unsigned head;
} edge;

typedef struct {
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
} Graph;

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

Graph* Graph_Create();

GraphResult Graph_AddVertex(Graph*, const unsigned id);
GraphResult Graph_AddEdge(Graph*, const unsigned tail, const unsigned head);
Graph* Graph_FromFile(const char* path);
GraphResult Graph_CollapseEdge(Graph*, size_t edge_index);
void Graph_Print(const Graph*);
Graph* Graph_Clone(const Graph*);

void Graph_Destroy(Graph*);
