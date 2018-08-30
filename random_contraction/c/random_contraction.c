#include <stdbool.h>

#include "random_contraction.h"

static bool graph_is_null(graph* graph)
{
    return graph == NULL || graph->vertex_count == 0 || graph->verticies == NULL
        || graph->edge_count == 0 || graph->edges == NULL;
}

int min_cut(graph* graph)
{
    if (graph_is_null(graph))
        return -1;

    return 0;
}
