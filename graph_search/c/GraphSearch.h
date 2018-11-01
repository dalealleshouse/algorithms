#pragma once

#include "Graph.h"

typedef bool (*search_query)(Vertex*);

typedef struct SearchStrategy {
    // marks a vertex as conquered
    search_query conqueror;
    // returns true if vertex is conquered, otherwise false
    search_query is_conquered;
} SearchStrategy;

GraphResult Graph_BFS(Graph*, int, SearchStrategy*);
GraphResult Graph_DFS(Graph*, int, SearchStrategy*);
