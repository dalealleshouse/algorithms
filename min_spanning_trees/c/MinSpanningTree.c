#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "include/DisjointSet.h"
#include "include/Heap.h"
#include "include/MemAllocMock.h"
#include "include/OverflowChecker.h"

#include "MinSpanningTree.h"

typedef struct {
  int score;
  int vertex_id;
  bool conquered;
  Edge* winner;
} SpanTreeResult;

static void _initMst(MinSpanTree* mst) {
  mst->cost = 0;
  mst->edge_count = 0;
  mst->edge_head = NULL;
  mst->edge_tail = NULL;
}

static int _edgeComparator(const void* x, const void* y) {
  Edge* _x = (Edge*)x;
  Edge* _y = (Edge*)y;

  if (_x->weight < _y->weight)
    return -1;
  else if (_x->weight > _y->weight)
    return 1;

  return 0;
}

static int _spanTreeResultComparator(const void* x, const void* y) {
  SpanTreeResult* _x = (SpanTreeResult*)x;
  SpanTreeResult* _y = (SpanTreeResult*)y;

  // The commented code and cause integer overflows (INT_MAX - -1)
  /* return _y->score - _x->score; */

  if (_x->score > _y->score)
    return -1;
  else if (_x->score < _y->score)
    return 1;

  return 0;
}

static void _conquer(Vertex* v, char* conqueredList) {
  conqueredList[v->id] = 1;
}

static bool _isConquered(Vertex* v, char* conqueredList) {
  return conqueredList[v->id] == 1;
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

MinSpanTree* MinSpanTree_Create() {
  MinSpanTree* mst = calloc(sizeof(MinSpanTree), 1);
  if (mst == NULL) return NULL;

  return mst;
}

static Edge* _findCheapestEdgeInGraph(const Graph* graph, char* conqueredList) {
  Edge* shortest = NULL;

  for (size_t i = 0; i < graph->n; i++) {
    Vertex* v = graph->V[i];

    // Consider edges where the tail is conquered
    if (!_isConquered(v, conqueredList)) continue;

    Edge* current = v->edges;

    while (current != NULL) {
      // The tail is conquered and the head is not
      if (!_isConquered(graph->V[current->head], conqueredList)) {
        if (shortest == NULL || shortest->weight > current->weight) {
          shortest = current;
        }
      }

      current = current->next;
    }
  }

  return shortest;
}

static Result _addEdgeToMst(MinSpanTree* mst, Edge* edge) {
  Edge* current = malloc(sizeof(Edge));
  if (current == NULL) return FailedMemoryAllocation;

  *current = *edge;
  current->next = NULL;

  if (mst->edge_head == NULL) {
    mst->edge_head = current;
    mst->edge_tail = current;
  } else {
    mst->edge_tail->next = current;
    mst->edge_tail = current;
  }

  if (is_add_overflow_int(mst->cost, current->weight)) {
    return ArithmeticOverflow;
  }

  ++mst->edge_count;
  mst->cost += current->weight;
  return Success;
}

void MinSpanTree_Edges_Destroy(Edge* edges) {
  if (edges == NULL) return;

  Edge* edge = edges;
  while (edge != NULL) {
    Edge* temp = edge;
    edge = temp->next;
    free(temp);
  }
}

void MinSpanTree_Destroy(MinSpanTree* mst) {
  if (mst == NULL) return;

  MinSpanTree_Edges_Destroy(mst->edge_head);

  free(mst);
}

Result PrimMinSpanTreeNaive(const Graph* graph, MinSpanTree* mst) {
  if (graph == NULL || mst == NULL) return NullParameter;

  _initMst(mst);

  char conquered[graph->n];
  // initalized to zero
  memset(&conquered, 0, graph->n * sizeof(char));

  Vertex* v = graph->V[1];
  _conquer(v, conquered);

  Edge* shortest = _findCheapestEdgeInGraph(graph, conquered);

  while (shortest != NULL) {
    Result result = _addEdgeToMst(mst, shortest);
    if (result != Success) return result;

    _conquer(graph->V[shortest->head], conquered);
    shortest = _findCheapestEdgeInGraph(graph, conquered);
  }

  return Success;
}

void _setWinner(SpanTreeResult* conquered, int vertex_id, Edge* winner) {
  if (winner == NULL) {
    conquered[vertex_id].score = INT_MAX;
    conquered[vertex_id].winner = NULL;
  } else {
    conquered[vertex_id].score = winner->weight;
    conquered[vertex_id].winner = winner;
  }
}

Edge* _findWinningEdge(SpanTreeResult* conquered, Edge* edges) {
  Edge* winner = NULL;
  Edge* edge = edges;

  // Find the cheapest edge where the tail is conquered and the head is not
  while (edge != NULL) {
    if (conquered[edge->head].conquered) {
      if (winner == NULL || winner->weight > edge->weight) winner = edge;
    }
    edge = edge->next;
  }

  return winner;
}

Result PrimMinSpanTree(const Graph* graph, MinSpanTree* mst) {
  if (graph == NULL || mst == NULL) return NullParameter;

  _initMst(mst);

  SpanTreeResult conquered[graph->n];
  // initialized to zero
  memset(&conquered, 0, graph->n * sizeof(conquered[0]));
  conquered[1].conquered = true;
  conquered[1].vertex_id = 1;

  Heap* heap = Heap_Create(graph->n, _spanTreeResultComparator);
  if (heap == NULL) return FailedMemoryAllocation;

  for (size_t i = 2; i < graph->n; i++) {
    conquered[i].vertex_id = i;

    Edge* winner = _findWinningEdge(conquered, graph->V[i]->edges);
    _setWinner(conquered, i, winner);

    HeapResult result = Heap_Insert(heap, &conquered[i]);
    if (result != HeapSuccess) {
      Heap_Destroy(heap, NULL);
      return DependancyError;
    }
  }

  while (!Heap_IsEmpty(heap)) {
    SpanTreeResult* next = Heap_Extract(heap);

    if (next == NULL) {
      Heap_Destroy(heap, NULL);
      return DependancyError;
    }

    Result result = _addEdgeToMst(mst, next->winner);
    if (result != Success) {
      Heap_Destroy(heap, NULL);
      return result;
    }

    conquered[next->vertex_id].conquered = true;

    Edge* recalcEdges = graph->V[next->vertex_id]->edges;
    while (recalcEdges != NULL) {
      int vertex_id = recalcEdges->head;
      if (!conquered[vertex_id].conquered) {
        Edge* winner = _findWinningEdge(conquered, graph->V[vertex_id]->edges);
        _setWinner(conquered, vertex_id, winner);

        Heap_Reproiritize(heap, &conquered[recalcEdges->head]);
      }
      recalcEdges = recalcEdges->next;
    }
  }

  Heap_Destroy(heap, NULL);
  return Success;
}

Result KruskalMinSpanTree(const Graph* graph, MinSpanTree* mst) {
  if (graph == NULL || mst == NULL) return NullParameter;

  _initMst(mst);

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
    // n-1 means we are done
    if (mst->edge_count == graph->n - 1) break;

    bool hasCycle;
    result = _hasCycle(&ds, graph, &edges[i], &hasCycle);
    if (result != Success) {
      DisjointSet_Destory(&ds);
      return result;
    }

    if (hasCycle)
      continue;
    else {
      result = DisjointSet_Union(&ds, graph->V[edges[i].head],
                                 graph->V[edges[i].tail]);
      if (result != Success) {
        DisjointSet_Destory(&ds);
        return result;
      }

      Result result = _addEdgeToMst(mst, &edges[i]);
      if (result != Success) {
        DisjointSet_Destory(&ds);
        return result;
      }
    }
  }

  DisjointSet_Destory(&ds);
  return Success;
}
