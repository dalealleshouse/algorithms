#pragma once

#include "include/ResultCode.h"
#include "include/Graph.h"

const int UNINITIALIZED;

typedef struct Distance {
    int distance;
} Distance;

GraphResult BellmanFordShortestPath(Graph* graph, const vertex_id start_vertex);
