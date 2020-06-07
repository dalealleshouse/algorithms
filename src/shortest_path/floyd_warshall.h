#pragma once

#include "../utils/graph.h"
#include "../utils/result_code.h"
#include "./constants.h"

GraphResult FloydWarshallShortestPath(Graph* graph, path_length** solutions);
