#pragma once

#include "include/Graph.h"
#include "include/ResultCode.h"

Result KruskalCluster(
    const Graph* graph, const size_t num_clusters, int* max_space);
