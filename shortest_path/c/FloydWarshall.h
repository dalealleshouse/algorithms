#pragma once

#include "include/Graph.h"
#include "include/ResultCode.h"

typedef double path_length;
const path_length UINIT_PATH;

GraphResult FloydWarshallShortestPath(Graph* graph, path_length** solutions);
