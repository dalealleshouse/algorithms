#include "./kruskal_cluster.h"

#include <stdlib.h>

#include "../data_structures/disjoint_set.h"
#include "../utils/overflow_checker.h"

static int _edgeComparator(const void* x, const void* y) {
  Edge* _x = (Edge*)x;
  Edge* _y = (Edge*)y;

  if (_x->weight < _y->weight) {
    return -1;
  } else if (_x->weight > _y->weight) {
    return 1;
  }

  return 0;
}

static Result _hasCycle(DisjointSet* ds, const Graph* graph, const Edge* edge,
                        bool* result) {
  SetItem* tail;
  SetItem* head;

  Result r = DisjointSet_FindSet(ds, graph->V[edge->tail], &tail);
  if (r != Success) return r;

  r = DisjointSet_FindSet(ds, graph->V[edge->head], &head);
  if (r != Success) return r;

  *result = tail == head;
  return Success;
}

Result KruskalCluster(const Graph* graph, const size_t num_clusters,
                      int* max_space) {
  if (graph == NULL) return NullParameter;

  if (num_clusters <= 1) return ArgumentOutOfRange;

  Edge edges[graph->m];

  DisjointSet ds;
  Result result = DisjointSet_Init(&ds, graph->n);
  if (result != Success) return result;

  // Initialize the disjoint set and edge array
  size_t current_edge = 0;
  for (size_t i = 1; i < graph->n; i++) {
    SetItem* item;
    DisjointSet_MakeSet(&ds, graph->V[i], &item);

    Edge* edge = graph->V[i]->edges;
    while (edge != NULL) {
      edges[current_edge] = *edge;
      edge = edge->next;
      ++current_edge;
    }
  }

  // Sort the edges with the cheapest edges at the top
  qsort(edges, graph->m, sizeof(Edge), _edgeComparator);

  for (size_t i = 0; i < graph->m; i++) {
    if (ds.num_sets < num_clusters) {
      *max_space = edges[i].weight;
      break;
    }

    bool hasCycle;
    result = _hasCycle(&ds, graph, &edges[i], &hasCycle);
    if (result != Success) {
      DisjointSet_Destory(&ds);
      return result;
    }

    if (hasCycle) {
      continue;
    } else {
      result = DisjointSet_Union(&ds, graph->V[edges[i].head],
                                 graph->V[edges[i].tail]);
      if (result != Success) {
        DisjointSet_Destory(&ds);
        return result;
      }

      if (result != Success) {
        DisjointSet_Destory(&ds);
        return result;
      }
    }
  }

  DisjointSet_Destory(&ds);
  return Success;
}
