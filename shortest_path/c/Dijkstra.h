#pragma once

#include "include/Graph.h"

typedef struct ShortestPath {
    int distance;
    Vertex* previous;
} ShortestPath;

GraphResult Graph_DijkstraShortestPath(Graph*, int);
