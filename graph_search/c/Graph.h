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
    // Id of the vertex
    unsigned vertex_id;

    // data associated with the vertex
    void* data;

    // Number of edges connected to the vertex
    size_t degree;

    struct VertexPrivate* _;
} Vertex;

typedef struct {
    // Id of the vertex at the tail of the edge
    unsigned tail;

    // Id of the vertex at the head of the edge
    unsigned head;
} Edge;

typedef struct {
    // Number of vertices
    size_t n;

    // Vertices
    Vertex* V;

    // Number of edges
    size_t m;

    // Edges
    Edge* E;

    // Private graph members
    struct GraphPrivate* _;
} Graph;

GraphResult Graph_Init(Graph*);

GraphResult Graph_AddVertex(Graph*, const unsigned id);
GraphResult Graph_AddEdge(Graph*, const unsigned tail, const unsigned head);
GraphResult Graph_FromFile(Graph*, const char* path);
void Graph_Print(const Graph*, FILE* file);

void Graph_Destroy(Graph*);
