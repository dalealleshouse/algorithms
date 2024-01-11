#pragma once

#include "graph.h"

typedef int pos_path_length;

typedef struct ShortestPath {
  pos_path_length distance;
  Vertex* previous;
  bool conquered;
} ShortestPath;

GraphResult Graph_DijkstraShortestPathNaive(Graph*, int);
GraphResult Graph_DijkstraShortestPath(Graph*, int);
