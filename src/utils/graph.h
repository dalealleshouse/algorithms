#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "./common.h"
#include "./error_reporter.h"

typedef unsigned int vertex_id;

#define MAX_ERROR_SIZE 1000
// We may need a better way to log errors, but this will work for now
#define GRAPH_ERROR(result)                                                 \
  {                                                                         \
    char str[MAX_ERROR_SIZE];                                               \
    snprintf(str, MAX_ERROR_SIZE, "Graph Error: %s, %s, %s, %d\n",          \
             Graph_ErrorMessage(result), __FILE__, __FUNCTION__, __LINE__); \
                                                                            \
    ErrorReporter_Report(result, str);                                      \
  }

typedef enum GraphResult {
  Graph_ArithmeticOverflow = -10,
  Graph_NegativeCycle = -9,
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
  vertex_id tail;

  vertex_id head;

  int weight;

  struct Edge* next;
} Edge;

typedef struct Vertex {
  // Id of the vertex
  vertex_id id;

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
GraphResult Graph_AddEdge(Graph*, vertex_id, vertex_id);
GraphResult Graph_AddWeightedEdge(Graph*, vertex_id, vertex_id, int);
Graph* Graph_FromFile(const size_t, const char* path);
Graph* Graph_WeightedFromFile(const size_t, const char* path);
Graph* Graph_WeightedFromFileUndirected(const char* path);
Graph* Graph_WeightedFromFileDirected(const char* path);

// Function of Mass Destruction
void Graph_Destroy(Graph*, freer);

char* Graph_ErrorMessage(GraphResult);
