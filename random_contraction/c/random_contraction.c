#include <math.h>
#include <stdlib.h>

#include "random_contraction.h"

static bool graph_is_invalid(const Graph* graph) {
  return graph == NULL || graph->n < 2 || graph->V == NULL || graph->m < 1 ||
         graph->E == NULL;
}

Graph* _CollaspeTo(Graph* graph, size_t size) {
  while (graph->n > size && graph->m > 1) {
    int edge_index = rand() % (graph->m - 1);
    Graph_CollapseEdge(graph, edge_index);
  }

  return graph;
}

Graph* RC_KargerMinCut(const Graph* input) {
  if (graph_is_invalid(input)) return NULL;

  Graph* min = NULL;
  int iterations = (input->n * input->n) * log(input->n);

  for (int i = 0; i < iterations; i++) {
    Graph* graph = Graph_Clone(input);

    _CollaspeTo(graph, 2);

    if (min == NULL || graph->m < min->m) {
      Graph_Destroy(min);
      min = graph;
    } else
      Graph_Destroy(graph);
  }

  return min;
}

Graph* _RC_KargerSteinMinCut(const Graph* input) {
  if (input->n <= 6) return RC_KargerMinCut(input);

  int t = (input->n + 1) / sqrt(2);
  Graph* g1 = Graph_Clone(input);
  _CollaspeTo(g1, t);
  Graph* g2 = Graph_Clone(g1);

  Graph* n_g1 = _RC_KargerSteinMinCut(g1);
  Graph* n_g2 = _RC_KargerSteinMinCut(g2);

  Graph_Destroy(g1);
  Graph_Destroy(g2);

  if (n_g1->m < n_g2->m) {
    Graph_Destroy(n_g2);
    return n_g1;
  }

  Graph_Destroy(n_g1);
  return n_g2;
}

Graph* RC_KargerSteinMinCut(const Graph* input) {
  if (graph_is_invalid(input)) return NULL;

  Graph* min = NULL;
  /* int iterations = (input->n * log(input->n)) / (input->n - 1); */
  int iterations = log(input->n) * log(input->n);
  for (int i = 0; i < iterations; i++) {
    Graph* result = _RC_KargerSteinMinCut(input);

    if (min == NULL || result->m < min->m) {
      Graph_Destroy(min);
      min = result;
    } else
      Graph_Destroy(result);
  }

  return min;
}
