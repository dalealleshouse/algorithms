#include "random_contraction.h"

#include <math.h>
#include <stdlib.h>

static bool graph_is_invalid(const Deprecated_Graph* graph) {
  return graph == NULL || graph->n < 2 || graph->V == NULL || graph->m < 1 ||
         graph->E == NULL;
}

Deprecated_Graph* _CollaspeTo(Deprecated_Graph* graph, size_t size) {
  while (graph->n > size && graph->m > 1) {
    int edge_index = random() % (graph->m - 1);
    Graph_CollapseEdge(graph, edge_index);
  }

  return graph;
}

Deprecated_Graph* RC_KargerMinCut(const Deprecated_Graph* input) {
  if (graph_is_invalid(input)) return NULL;

  Deprecated_Graph* min = NULL;
  int iterations = (input->n * input->n) * log(input->n);

  for (int i = 0; i < iterations; i++) {
    Deprecated_Graph* graph = Graph_Clone(input);

    _CollaspeTo(graph, 2);

    if (min == NULL || graph->m < min->m) {
      Deprecated_Graph_Destroy(min);
      min = graph;
    } else {
      Deprecated_Graph_Destroy(graph);
    }
  }

  return min;
}

Deprecated_Graph* _RC_KargerSteinMinCut(const Deprecated_Graph* input) {
  if (input->n <= 6) return RC_KargerMinCut(input);

  int t = (input->n + 1) / sqrt(2);
  Deprecated_Graph* g1 = Graph_Clone(input);
  _CollaspeTo(g1, t);
  Deprecated_Graph* g2 = Graph_Clone(g1);

  Deprecated_Graph* n_g1 = _RC_KargerSteinMinCut(g1);
  Deprecated_Graph* n_g2 = _RC_KargerSteinMinCut(g2);

  Deprecated_Graph_Destroy(g1);
  Deprecated_Graph_Destroy(g2);

  if (n_g1->m < n_g2->m) {
    Deprecated_Graph_Destroy(n_g2);
    return n_g1;
  }

  Deprecated_Graph_Destroy(n_g1);
  return n_g2;
}

Deprecated_Graph* RC_KargerSteinMinCut(const Deprecated_Graph* input) {
  if (graph_is_invalid(input)) return NULL;

  Deprecated_Graph* min = NULL;
  /* int iterations = (input->n * log(input->n)) / (input->n - 1); */
  int iterations = log(input->n) * log(input->n);
  for (int i = 0; i < iterations; i++) {
    Deprecated_Graph* result = _RC_KargerSteinMinCut(input);

    if (min == NULL || result->m < min->m) {
      Deprecated_Graph_Destroy(min);
      min = result;
    } else {
      Deprecated_Graph_Destroy(result);
    }
  }

  return min;
}
