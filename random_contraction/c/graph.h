#pragma once

#include <stdbool.h>
#include <stdio.h>

// Initial size of vertex and edge allocation
const size_t INITIAL_ALLOC;

// Factor by when to increase allocation size when more space is required
const size_t REALLOC_FACTOR;

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

Graph* Graph_Create();

GraphResult Graph_AddVertex(Graph*, const unsigned id);
GraphResult Graph_AddEdge(Graph*, const unsigned tail, const unsigned head);
Graph* Graph_FromFile(const char* path);
GraphResult Graph_CollapseEdge(Graph*, size_t edge_index);
void Graph_Print(const Graph*, FILE* file);
Graph* Graph_Clone(const Graph*);

void Graph_Destroy(Graph*);
