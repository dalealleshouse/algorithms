#include <stdbool.h>

#include "random_contraction.h"

static bool graph_is_invalid(graph* graph)
{
    return graph == NULL || graph->vertex_count == 0 || graph->verticies == NULL
        || graph->edge_count == 0 || graph->edges == NULL;
}

int collapse_edge(graph* graph, size_t edge_index){
    (void)graph;
    (void)edge_index;


    return -1;
}

int min_cut(graph* graph)
{
    if (graph_is_invalid(graph))
        return -1;

    return 0;
}
