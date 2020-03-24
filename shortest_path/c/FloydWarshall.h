#pragma once

#include "include/Graph.h"
#include "include/ResultCode.h"

#include "constants.h"

GraphResult FloydWarshallShortestPath(Graph* graph, path_length** solutions);
