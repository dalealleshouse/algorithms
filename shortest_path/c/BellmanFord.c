#include <limits.h>
#include <stdlib.h>

#include "include/OverflowChecker.h"

#include "BellmanFord.h"

const int UNINITIALIZED = INT_MAX;

static GraphResult _shortest(size_t n, Vertex* v, int cache[n], int* result) {
  int winner = UNINITIALIZED;

  Edge* e = v->in_edges;
  while (e != NULL) {
    int contender = UNINITIALIZED;

    if (cache[e->tail] != UNINITIALIZED) {
      if (is_add_overflow_int(cache[e->tail], e->weight))
        return Graph_ArithmeticOverflow;

      contender = cache[e->tail] + e->weight;
    }

    winner = (contender < winner) ? contender : winner;
    e = e->next;
  }

  *result = winner;
  return Graph_Success;
}

GraphResult _loadDistances(Graph* graph, int answers[graph->n]) {
  for (size_t i = 0; i < graph->n; i++) {
    Distance* d = malloc(sizeof(Distance));
    if (d == NULL) return Graph_FailedMemoryAllocation;

    d->distance = answers[i];
    graph->V[i]->data = d;
  }

  return Graph_Success;
}

GraphResult BellmanFordShortestPath(Graph* graph,
                                    const vertex_id start_vertex) {
  if (graph == NULL) return Graph_NullParameter;
  if (start_vertex >= graph->n) return Graph_InvalidVertexId;

  int cache[graph->n + 1][graph->n];

  for (size_t i = 0; i < graph->n; i++) cache[0][i] = UNINITIALIZED;
  cache[0][start_vertex] = 0;

  for (size_t i = 1; i < graph->n; i++) {
    bool stable = true;
    for (size_t j = 0; j < graph->n; j++) {
      int case1 = cache[i - 1][j];
      int case2 = UNINITIALIZED;
      GraphResult result =
          _shortest(graph->n, graph->V[j], cache[i - 1], &case2);

      if (result != Graph_Success) return result;

      cache[i][j] = (case1 < case2) ? case1 : case2;

      if (cache[i][j] != cache[i - 1][j]) stable = false;
    }

    if (stable) return _loadDistances(graph, cache[i]);
  }

  return Graph_NegativeCycle;
}
