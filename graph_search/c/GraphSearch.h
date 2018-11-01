#pragma once

#include "Graph.h"

typedef struct VertexData {
    size_t shortest_path;
    bool am_i_conquered;
} VertexData;

typedef struct SearchStrategy {
    // marks a vertex as conquered, accepts
    bool (*conqueror)(Vertex* current, Vertex* previous);
    // returns true if vertex is conquered, otherwise false
    bool (*is_conquered)(Vertex*);
} SearchStrategy;

GraphResult Graph_BFS(Graph*, int, SearchStrategy*);
GraphResult Graph_Reachable(Graph*, int);
GraphResult Graph_ShortestPath(Graph*, int);
GraphResult Graph_DFS(Graph*, int, SearchStrategy*);
