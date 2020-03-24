#pragma once

#include "include/Graph.h"
#include "include/ResultCode.h"

#include "constants.h"

typedef struct BFData {
  path_length distance;
  Vertex* prev;
} BFData;

typedef struct Path {
  vertex_id vertex;
  struct Path* next;
  size_t n;
} Path;

GraphResult BellmanFordShortestPath(Graph* graph, const vertex_id start_vertex);
GraphResult BellmandFordTracePath(const Graph* graph, const vertex_id start,
                                  Path** path);
void Path_Destroy(Path* path);
