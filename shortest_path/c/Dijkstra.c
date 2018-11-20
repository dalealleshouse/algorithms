#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "Dijkstra.h"
#include "include/MemAllocMock.h"
#include "include/PriorityQueue.h"

static GraphResult InitShortestPath(Vertex* v)
{
    if (v == NULL)
        return Graph_NullParameter;

    if (v->data == NULL) {
        v->data = calloc(sizeof(ShortestPath), 1);

        if (v->data == NULL)
            return Graph_FailedMemoryAllocation;

        ((ShortestPath*)v->data)->distance = INFINITY;
        ((ShortestPath*)v->data)->conquered = false;
    }

    return Graph_Success;
}

static double Distance(Vertex* v)
{
    if (v == NULL || v->data == NULL)
        return INFINITY;

    return ((ShortestPath*)v->data)->distance;
}

static int path_comparator(const void* x, const void* y)
{
    double xd = Distance((Vertex*)x);
    double yd = Distance((Vertex*)y);

    if (xd == yd)
        return 0;

    if (xd < yd)
        return 1;

    return -1;
}

static bool IsConquered(Vertex* v)
{
    if (v == NULL || v->data == NULL)
        return false;

    return ((ShortestPath*)v->data)->conquered;
}

static GraphResult Conquer(Vertex* v)
{
    GraphResult result = InitShortestPath(v);
    if (result != Graph_Success)
        return result;

    ShortestPath* sp = v->data;
    sp->conquered = true;

    return Graph_Success;
}

static GraphResult SetDistance(Vertex* v, size_t distance, Vertex* prev)
{
    GraphResult result = InitShortestPath(v);
    if (result != Graph_Success)
        return result;

    ShortestPath* sp = v->data;

    // Only update if it's less
    if (distance < sp->distance) {
        sp->distance = distance;
        sp->previous = prev;
    }

    return Graph_Success;
}

static GraphResult SetDistanceAndConquer(
    Vertex* v, size_t distance, Vertex* prev)
{
    GraphResult result = InitShortestPath(v);
    if (result != Graph_Success)
        return result;

    ShortestPath* sp = v->data;

    // Only update if it's less
    if (distance < sp->distance) {
        sp->distance = distance;
        sp->previous = prev;
    }

    sp->conquered = true;
    return Graph_Success;
}

static PriorityQueue* InitPQ(Graph* self)
{
    PriorityQueue* pq = PQ_Create(path_comparator);
    if (pq == NULL)
        return NULL;

    for (size_t i = 0; i < self->n; i++)
        PQ_Insert(pq, self->V[i]);

    return pq;
}

static Vertex* FindMin(Graph* self, Vertex* conquered[])
{
    Vertex* v = NULL;
    size_t tracker = 0;
    Vertex* curr = conquered[tracker];

    while (curr != NULL) {
        Edge* e = curr->edges;
        while (e != NULL) {
            Vertex* tail = self->V[e->tail];
            Vertex* head = self->V[e->head];
            if (!IsConquered(head)) {
                GraphResult r
                    = SetDistance(head, Distance(tail) + e->weight, tail);
                if (r != Graph_Success)
                    return NULL;

                if (Distance(head) < Distance(v))
                    v = head;
            }
            e = e->next;
        }
        curr = conquered[++tracker];
    }

    return v;
}

GraphResult Graph_DijkstraShortestPathNaive(Graph* self, int start)
{
    if (self == NULL)
        return Graph_NullParameter;

    if (start < 0 || (size_t)start >= self->n)
        return Graph_InvalidVertexId;

    size_t tracker = 0;
    Vertex** conquered = calloc(sizeof(Vertex*), self->n + 1);
    if (conquered == NULL)
        return Graph_FailedMemoryAllocation;

    Vertex* startv = self->V[start];
    GraphResult r = SetDistanceAndConquer(startv, 0, NULL);
    if (r != Graph_Success)
        return r;

    conquered[tracker] = startv;

    while (true) {
        Vertex* v = FindMin(self, conquered);
        if (Distance(v) == INFINITY)
            break;

        conquered[++tracker] = v;
        r = Conquer(v);
        if (r != Graph_Success)
            return r;
    }

    free(conquered);
    return Graph_Success;
}

GraphResult Graph_DijkstraShortestPath(Graph* self, int start)
{
    if (self == NULL)
        return Graph_NullParameter;

    if (start < 0 || (size_t)start >= self->n)
        return Graph_InvalidVertexId;

    Vertex* startv = self->V[start];
    GraphResult r = SetDistanceAndConquer(startv, 0, NULL);
    if (r != Graph_Success)
        return r;

    PriorityQueue* pq = PQ_Create(path_comparator);
    if (pq == NULL)
        return Graph_DependencyError;

    PQResult pqr = PQ_Insert(pq, startv);
    if (pqr != PQ_Success)
        return Graph_DependencyError;

    while (!PQ_IsEmpty(pq)) {
        Vertex* v = PQ_Remove(pq);

        // unreachable vertices, time to bail
        if (Distance(v) == INFINITY)
            break;

        Edge* e = v->edges;
        while (e != NULL) {
            Vertex* head = self->V[e->head];
            double dist = Distance(v) + e->weight;

            if (IsConquered(head)) {
                r = SetDistance(head, dist, v);
                if (r != Graph_Success)
                    return r;

                // We only need to re prioritize if the distance changed
                if (dist == Distance(head)) {
                    pqr = PQ_Reprioritize(pq, head);
                    if (pqr != PQ_Success)
                        return Graph_DependencyError;
                }
            } else {
                r = SetDistanceAndConquer(head, dist, v);
                if (r != Graph_Success)
                    return r;

                pqr = PQ_Insert(pq, head);
                if (pqr != PQ_Success)
                    return Graph_DependencyError;
            }
            e = e->next;
        }
    }

    PQ_Destroy(pq, NULL);
    return Graph_Success;
}
