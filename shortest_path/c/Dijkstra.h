#pragma once

#include "include/Graph.h"

typedef struct ShortestPath {
    double distance;
    Vertex* previous;
} ShortestPath;

GraphResult Graph_DijkstraShortestPath(Graph*, int);
