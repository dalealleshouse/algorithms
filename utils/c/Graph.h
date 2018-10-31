#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "CommonTypes.h"

typedef enum GraphResult {
    Graph_VertexIdExceedsMaxSize = -5,
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
    size_t max_size;

    // Vertices
    Vertex** V;

    // Number of edges - sum of degree of all vertices
    size_t m;
} Graph;

Graph* Graph_Create(size_t);
GraphResult Graph_AddVertex(Graph*, int, void*);
GraphResult Graph_AddEdge(Graph*, int, int);

// Function of Mass Destruction
void Graph_Destroy(Graph*, freer);

char* Graph_ErrorMessage(GraphResult);
