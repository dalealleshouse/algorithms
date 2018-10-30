#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "CommonTypes.h"

typedef enum GraphResult {
    Graph_DuplicateVertexId = -4,
    Graph_InvalidVertexId = -3,
    Graph_FailedMemoryAllocation = -2,
    Graph_NullParameter = -1,
    Graph_Success = 0
} GraphResult;

// Edges are a linked list
typedef struct Edge {
    // Id of the vertex at the head of the edge, the tail is vertex object
    // hosting this edge
    int head;

    struct Edge* next;
} Edge;

typedef struct Vertex {
    // Id of the vertex
    int id;

    // data associated with the vertex
    void* data;

    // Number of edges connected to the vertex
    size_t degree;

    // Linked list of edges associated with this vertex
    Edge* edges;
} Vertex;

typedef struct Graph {
    // Number of vertices
    size_t n;

    // Number of spaces allocated in V
    size_t n_allocated;

    // Vertices
    Vertex** V;

    // Number of edges - sum of degree of all vertices
    size_t m;

    // private members
    struct GraphPrivate* _;
} Graph;

// Initalize values to default
GraphResult Graph_Init(Graph*);
GraphResult Graph_VertexInit(Vertex*);
GraphResult Graph_EdgeInit(Edge*);

// Convenience methods that malloc and init objects
Graph* Graph_Create(void);
Vertex* Graph_VertexCreate(int, void*);
Edge* Graph_EdgeCreate(int);

GraphResult Graph_AddVertex(Graph*, Vertex*);
GraphResult Graph_AutoAddVertex(Graph*, int, void*);

GraphResult Graph_AddEdge(Vertex*, const Edge*);
GraphResult Graph_AutoAddEdge(Vertex*, unsigned);

GraphResult Graph_FromFile(Graph*, const char* path);
void Graph_Print(const Graph*, FILE* file);

// Functions of Mass Destruction
void Graph_Destroy(Graph*, freer);
void Graph_VertexDestroy(Vertex*, freer);
void Graph_EdgeDestroy(Edge*);

char* Graph_ErrorMessage(GraphResult);
