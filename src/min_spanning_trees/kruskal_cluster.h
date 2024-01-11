#pragma once

#include "graph.h"
#include "result_code.h"

Result KruskalCluster(const Graph* graph, const size_t num_clusters,
                      int* max_space);
