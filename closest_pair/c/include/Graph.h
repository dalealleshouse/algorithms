#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "CommonTypes.h"
#include "ErrorReporter.h"

// We may need a better way to log errors, but this will work for now
#define GRAPH_ERROR(result)                                                    \
    {                                                                          \
        char str[1000];                                                        \
        sprintf(str, "Graph Error: %s, %s, %s, %d\n",                          \
            Graph_ErrorMessage(result), __FILE__, __FUNCTION__, __LINE__);     \
                                                                               \
        ErrorReporter_Report(result, str);                                     \
    }

typedef enum GraphResult {
    Graph_DependencyError = -8,
    Graph_FileOpenError = -7,
    Graph_InvalidFilePath = -6,
    Graph_VertexIdExceedsMaxSize = -5,
    Graph_DuplicateVertexId = -4,
    Graph_InvalidVertexId = -3,
    Graph_FailedMemoryAllocation = -2,
    Graph_NullParameter = -1,
    Graph_Success = 0
} GraphResult;

typedef struct Edge {
    int tail;

    int head;

    int weight;

    struct Edge* next;
} Edge;

typedef struct Vertex {
    // Id of the vertex
    int id;

    // data associated with the vertex
    void* data;

    // Number of incoming edges
    size_t in_degree;

    // Number of outgoing edges
    size_t out_degree;

    // Linked list of edges associated with this vertex
    Edge* edges;

    Edge* in_edges;
} Vertex;

typedef struct Graph {
    // Number of vertices
    size_t n;

    // Vertices
    Vertex** V;

    // Number of edges - sum of degree of all vertices
    size_t m;
} Graph;

Graph* Graph_Create(size_t);
GraphResult Graph_AddEdge(Graph*, int, int);
GraphResult Graph_AddWeightedEdge(Graph*, int, int, int);
Graph* Graph_FromFile(const size_t, const char* path);
Graph* Graph_WeightedFromFile(const size_t, const char* path);

// Function of Mass Destruction
void Graph_Destroy(Graph*, freer);

char* Graph_ErrorMessage(GraphResult);
