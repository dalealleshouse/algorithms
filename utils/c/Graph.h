#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "CommonTypes.h"

typedef enum {
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

typedef struct {
    // Id of the vertex
    int id;

    // data associated with the vertex
    void* data;

    // Number of edges connected to the vertex
    size_t degree;

    // Linked list of edges associated with this vertex
    Edge* edges;
} Vertex;

typedef struct {
    // Number of vertices
    size_t n;

    // Vertices
    Vertex** V;

    // Number of edges - sum of degree of all vertices
    size_t m;
} Graph;

// The "Create" methods are convenience methods that malloc and init objects
GraphResult Graph_Init(Graph*);
Graph* Graph_Create(void);

GraphResult Graph_VertexInit(Vertex*);
Vertex* Graph_VertexCreate(int, void*);

GraphResult Graph_EdgeInit(Edge*);
Edge* Graph_EdgeCreate(int);

GraphResult Graph_AddVertex(Graph*, const Vertex*);
GraphResult Graph_AutoAddVertex(Graph*, int, void*);

GraphResult Graph_AddEdge(Vertex*, const Edge*);
GraphResult Graph_AutoAddEdge(Vertex*, unsigned);

GraphResult Graph_FromFile(Graph*, const char* path);
void Graph_Print(const Graph*, FILE* file);

// Functions of Mass Destruction
void Graph_Destroy(Graph*, freer);
void Graph_VertexDestroy(Vertex*, freer);

char* Graph_ErrorMessage(GraphResult);
