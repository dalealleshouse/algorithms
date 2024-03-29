#include "dijkstra.h"

#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "heap.h"

static GraphResult InitShortestPath(Vertex* v) {
  if (v == NULL) return Graph_kNullParameter;

  if (v->data == NULL) {
    v->data = calloc(sizeof(ShortestPath), 1);

    if (v->data == NULL) return Graph_kFailedMemoryAllocation;

    ((ShortestPath*)v->data)->distance = UNINITIALIZED;
    ((ShortestPath*)v->data)->conquered = false;
  }

  return Graph_kSuccess;
}

static double Distance(Vertex* v) {
  if (v == NULL || v->data == NULL) return UNINITIALIZED;

  return ((ShortestPath*)v->data)->distance;
}

static int path_comparator(const void* x, const void* y) {
  double xd = Distance((Vertex*)x);
  double yd = Distance((Vertex*)y);

  if (xd == yd) return 0;

  if (xd < yd) return 1;

  return -1;
}

static bool IsConquered(Vertex* v) {
  if (v == NULL || v->data == NULL) return false;

  return ((ShortestPath*)v->data)->conquered;
}

static GraphResult Conquer(Vertex* v) {
  GraphResult result = InitShortestPath(v);
  if (result != Graph_kSuccess) return result;

  ShortestPath* sp = v->data;
  sp->conquered = true;

  return Graph_kSuccess;
}

static GraphResult SetDistance(Vertex* v, pos_path_length distance,
                               Vertex* prev) {
  GraphResult result = InitShortestPath(v);
  if (result != Graph_kSuccess) return result;

  ShortestPath* sp = v->data;

  // Only update if it's less
  if (distance < sp->distance) {
    sp->distance = distance;
    sp->previous = prev;
  }

  return Graph_kSuccess;
}

static GraphResult SetDistanceAndConquer(Vertex* v, pos_path_length distance,
                                         Vertex* prev) {
  GraphResult result = InitShortestPath(v);
  if (result != Graph_kSuccess) return result;

  ShortestPath* sp = v->data;

  // Only update if it's less
  if (distance < sp->distance) {
    sp->distance = distance;
    sp->previous = prev;
  }

  sp->conquered = true;
  return Graph_kSuccess;
}

static Vertex* FindMin(Graph* self, Vertex* conquered[]) {
  Vertex* v = NULL;
  size_t tracker = 0;
  Vertex* curr = conquered[tracker];

  while (curr != NULL) {
    Edge* e = curr->edges;
    while (e != NULL) {
      Vertex* tail = self->V[e->tail];
      Vertex* head = self->V[e->head];
      if (!IsConquered(head)) {
        GraphResult r = SetDistance(head, Distance(tail) + e->weight, tail);
        if (r != Graph_kSuccess) return NULL;

        if (Distance(head) < Distance(v)) v = head;
      }
      e = e->next;
    }
    curr = conquered[++tracker];
  }

  return v;
}

GraphResult Graph_DijkstraShortestPathNaive(Graph* self, int start) {
  if (self == NULL) return Graph_kNullParameter;

  if (start < 0 || (size_t)start >= self->n) return Graph_InvalidVertexId;

  size_t tracker = 0;
  Vertex** conquered = calloc(sizeof(Vertex*), self->n + 1);
  if (conquered == NULL) return Graph_kFailedMemoryAllocation;

  Vertex* startv = self->V[start];
  GraphResult r = SetDistanceAndConquer(startv, 0, NULL);
  if (r != Graph_kSuccess) {
    free(conquered);
    return r;
  }

  conquered[tracker] = startv;

  while (true) {
    Vertex* v = FindMin(self, conquered);
    if (Distance(v) == UNINITIALIZED) break;

    conquered[++tracker] = v;
    r = Conquer(v);
    if (r != Graph_kSuccess) return r;
  }

  free(conquered);
  return Graph_kSuccess;
}

GraphResult Graph_DijkstraShortestPath(Graph* self, int start) {
  if (self == NULL) return Graph_kNullParameter;

  if (start < 0 || (size_t)start >= self->n) return Graph_InvalidVertexId;

  Vertex* startv = self->V[start];
  GraphResult r = SetDistanceAndConquer(startv, 0, NULL);
  if (r != Graph_kSuccess) return r;

  Heap* heap = NULL;
  ResultCode result_code = Heap_Create(self->n, path_comparator, &heap);
  if (result_code != kSuccess) return Graph_DependencyError;

  ResultCode h_result = Heap_Insert(heap, startv);
  if (h_result != kSuccess) return Graph_DependencyError;

  while (!Heap_IsEmpty(heap)) {
    Vertex* v = NULL;
    Heap_Extract(heap, (void**)&v);

    // unreachable vertices, time to bail
    if (Distance(v) == UNINITIALIZED) break;

    Edge* e = v->edges;
    while (e != NULL) {
      Vertex* head = self->V[e->head];
      double dist = Distance(v) + e->weight;

      if (IsConquered(head)) {
        r = SetDistance(head, dist, v);
        if (r != Graph_kSuccess) return r;

        // We only need to re prioritize if the distance changed
        if (dist == Distance(head)) {
          h_result = Heap_Reproiritize(heap, head);
          if (h_result != kSuccess) return Graph_DependencyError;
        }
      } else {
        r = SetDistanceAndConquer(head, dist, v);
        if (r != Graph_kSuccess) return r;

        h_result = Heap_Insert(heap, head);
        if (h_result != kSuccess) return Graph_DependencyError;
      }
      e = e->next;
    }
  }

  Heap_Destroy(heap, NULL);
  return Graph_kSuccess;
}
