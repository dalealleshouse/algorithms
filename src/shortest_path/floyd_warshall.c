#include "floyd_warshall.h"

#include <stdlib.h>

static void _printSolutions(size_t n, path_length* solutions) {
  printf("\n");
  for (size_t v = 0; v < n; v++) {
    printf("Vertex %zu=", v);
    for (size_t w = 0; w < n; w++) {
      printf("%12d ", solutions[v + n * w]);
    }
    printf("\n");
  }

  printf("         ");
  for (size_t w = 0; w < n; w++) printf("%12zu ", w);
}

path_length _findEdge(Graph* graph, vertex_id v, vertex_id w) {
  Edge* e = graph->V[v]->in_edges;
  path_length weight = UNINITIALIZED;

  // There can be multiple edge with the same head and tail so we have to find
  // the shortest one
  while (e != NULL) {
    if (e->tail == w) weight = (weight < e->weight) ? weight : e->weight;

    e = e->next;
  }

  return weight;
}

GraphResult FloydWarshallShortestPath(Graph* graph, path_length** solutions) {
  if (graph == NULL) return Graph_kNullParameter;

  size_t n = graph->n;

  path_length* current = malloc(sizeof(path_length) * n * n);
  path_length* previous = malloc(sizeof(path_length) * n * n);

  for (size_t v = 0; v < graph->n; v++) {
    for (size_t w = 0; w < graph->n; w++) {
      if (v == w) {
        current[v + n * w] = 0;
        continue;
      }

      current[v + n * w] = _findEdge(graph, v, w);
    }
  }

  for (size_t k = 0; k < n; k++) {
    path_length* temp = current;
    current = previous;
    previous = temp;

    for (size_t v = 0; v < n; v++) {
      for (size_t w = 0; w < n; w++) {
        path_length case1 = previous[v + n * w];
        path_length case2 = UNINITIALIZED;

        if (previous[v + n * k] != UNINITIALIZED &&
            previous[k + n * w] != UNINITIALIZED) {
          if (__builtin_add_overflow(previous[v + n * k], previous[k + n * w],
                                     &case2)) {
            free(previous);
            free(current);
            return Graph_kArithmeticOverflow;
          }
        }

        current[v + n * w] = (case1 < case2) ? case1 : case2;
        if (v == w && current[v + n * w] < 0) {
          free(previous);
          free(current);
          return Graph_NegativeCycle;
        }
      }
    }
  }
  free(previous);
  *solutions = current;
  (void)_printSolutions;

  return Graph_kSuccess;
}
