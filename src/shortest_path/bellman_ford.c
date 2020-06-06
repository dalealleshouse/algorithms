#include "./bellman_ford.h"

#include <limits.h>
#include <stdlib.h>

#include "../utils/overflow_checker.h"

static GraphResult _initPath(vertex_id vertex, Path* next, Path** path) {
  Path* p = malloc(sizeof(Path));

  if (p == NULL) return Graph_FailedMemoryAllocation;

  p->vertex = vertex;
  p->next = NULL;
  p->next = next;
  p->n = (next == NULL) ? 1 : next->n + 1;
  *path = p;

  return Graph_Success;
}

static vertex_id _getPrevious(Vertex* v) {
  BFData* data = v->data;

  if (data == NULL || data->prev == NULL || v->id == data->prev->id) {
    return UINT_MAX;
  }
  return data->prev->id;
}

static GraphResult _shortest(Graph* graph, Vertex* v, BFData cache[graph->n],
                             BFData* result) {
  BFData winner = {UNINITIALIZED, NULL};

  Edge* e = v->in_edges;
  while (e != NULL) {
    BFData contender = {UNINITIALIZED, NULL};

    if (cache[e->tail].distance != UNINITIALIZED) {
      if (is_add_overflow_int(cache[e->tail].distance, e->weight)) {
        return Graph_ArithmeticOverflow;
      }

      contender.distance = cache[e->tail].distance + e->weight;
      contender.prev = graph->V[e->tail];
    }

    winner = (contender.distance < winner.distance) ? contender : winner;
    e = e->next;
  }

  *result = winner;
  return Graph_Success;
}

GraphResult _loadBFDatas(Graph* graph, BFData answers[graph->n]) {
  for (size_t i = 0; i < graph->n; i++) {
    BFData* d = malloc(sizeof(BFData));
    if (d == NULL) return Graph_FailedMemoryAllocation;

    *d = answers[i];
    graph->V[i]->data = d;
  }

  return Graph_Success;
}

GraphResult BellmanFordShortestPath(Graph* graph,
                                    const vertex_id start_vertex) {
  if (graph == NULL) return Graph_NullParameter;
  if (start_vertex >= graph->n) return Graph_InvalidVertexId;

  BFData cache1[graph->n];
  BFData cache2[graph->n];
  BFData* previous = &cache1[0];
  BFData* current = &cache2[0];

  for (size_t i = 0; i < graph->n; i++) {
    previous[i] = (BFData){.distance = UNINITIALIZED, .prev = NULL};
  };

  previous[start_vertex] =
      (BFData){.distance = 0, .prev = graph->V[start_vertex]};

  // Each outer loop indicates the number of hops allowed in each path.
  // No path can have greater than n-1 hops
  for (size_t i = 1; i < graph->n; i++) {
    bool stable = true;
    for (size_t j = 0; j < graph->n; j++) {
      BFData case1 = previous[j];
      BFData case2 = {UNINITIALIZED, NULL};
      GraphResult result = _shortest(graph, graph->V[j], previous, &case2);
      if (result != Graph_Success) return result;

      // TODO(dalealleshouse): cache both the predecessor node and the value so
      // consumers can decipher the shortest path instead of just it's value
      current[j] = (case1.distance < case2.distance) ? case1 : case2;

      if (current[j].distance != previous[j].distance) stable = false;
    }

    if (stable) return _loadBFDatas(graph, current);
    BFData* temp = current;
    current = previous;
    previous = temp;
  }

  return Graph_NegativeCycle;
}

GraphResult BellmandFordTracePath(const Graph* graph, const vertex_id start,
                                  Path** path) {
  if (graph == NULL || path == NULL) return Graph_NullParameter;
  if (start >= graph->n) return Graph_VertexIdExceedsMaxSize;

  Path* p = NULL;
  vertex_id vid = start;
  while (vid != UINT_MAX) {
    GraphResult result = _initPath(vid, p, &p);
    if (result != Graph_Success) {
      Path_Destroy(p);
      return result;
    }

    vid = _getPrevious(graph->V[vid]);
  }

  *path = p;
  return Graph_Success;
}

void Path_Destroy(Path* path) {
  while (path != NULL) {
    Path* temp = path;
    path = path->next;
    free(temp);
  }
}
