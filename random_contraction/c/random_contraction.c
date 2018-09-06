#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "random_contraction.h"

static bool graph_is_invalid(const Graph* graph)
{
    return graph == NULL || graph->n < 2 || graph->V == NULL || graph->m < 1
        || graph->E == NULL;
}

int min_cut(const Graph* input)
{
    if (graph_is_invalid(input))
        return -1;

    Graph* min = NULL;
    int iterations = (input->n * input->n) * log(input->n);

    for (int i = 0; i < iterations; i++) {
        printf("iteration %d of %d\n", i, iterations);
        Graph* graph = Graph_Clone(input);

        while (graph->n > 2 && graph->m > 1) {
            int edge_index = rand() % (graph->m - 1);
            Graph_CollapseEdge(graph, edge_index);
        }

        if (min == NULL || graph->m < min->m) {
            Graph_Destroy(min);
            min = graph;
        } else
            Graph_Destroy(graph);
    }

    int result = min->m;
    Graph_Print(min);
    Graph_Destroy(min);
    return result;
}
