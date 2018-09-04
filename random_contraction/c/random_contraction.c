#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "random_contraction.h"

static bool graph_is_invalid(const graph* graph)
{
    return graph == NULL || graph->n < 2 || graph->V == NULL || graph->m < 1
        || graph->E == NULL;
}

int min_cut(const graph* input)
{
    if (graph_is_invalid(input))
        return -1;

    graph* min = NULL;
    int iterations = (input->n * input->n) * log(input->n);

    for (int i = 0; i < iterations; i++) {
        printf("iteration %d of %d\n", i, iterations);
        graph* graph = graph_clone(input);

        while (graph->n > 2 && graph->m > 1) {
            int edge_index = rand() % (graph->m - 1);
            graph_collapse_edge(graph, edge_index);
        }

        if (min == NULL || graph->m < min->m) {
            graph_destroy(min);
            min = graph;
        } else
            graph_destroy(graph);
    }

    int result = min->m;
    graph_print(min);
    graph_destroy(min);
    return result;
}
