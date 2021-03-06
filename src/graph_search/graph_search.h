#pragma once

#include "../data_structures/stack.h"
#include "../utils/graph.h"

typedef struct VertexData {
  int value;
  bool am_i_conquered;
} VertexData;

typedef struct SearchStrategy {
  // marks a vertex as conquered, accepts
  bool (*conqueror)(Vertex* current, Vertex* previous);
  // returns true if vertex is conquered, otherwise false
  bool (*is_conquered)(Vertex*);
} SearchStrategy;

GraphResult Graph_BFS(Graph*, int, SearchStrategy*);
GraphResult Graph_DFS(Graph*, int, SearchStrategy*);
GraphResult Graph_Reachable(Graph*, int);
GraphResult Graph_Connected(Graph*);
GraphResult Graph_TopSort(Graph*);
Stack* Graph_SCC_MagicOrdering(Graph*);
GraphResult Graph_SCC(Graph*);
