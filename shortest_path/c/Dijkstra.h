#pragma once

#include "include/Graph.h"

typedef struct ShortestPath {
    double distance;
    Vertex* previous;
    bool conquered;
} ShortestPath;

GraphResult Graph_DijkstraShortestPathNaive(Graph*, int);
GraphResult Graph_DijkstraShortestPath(Graph*, int);
