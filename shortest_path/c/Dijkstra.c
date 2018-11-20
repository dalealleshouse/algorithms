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
                SetDistance(head, Distance(tail) + e->weight, tail);
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

    size_t tracker = 1;
    Vertex** conquered = calloc(sizeof(Vertex*), self->n + 1);
    if (conquered == NULL)
        return Graph_FailedMemoryAllocation;

    Vertex* startv = self->V[start];
    GraphResult r = SetDistance(startv, 0, NULL);
    if (r != Graph_Success)
        return r;

    conquered[0] = startv;
    r = Conquer(startv);
    if (r != Graph_Success)
        return r;

    while (true) {
        Vertex* v = FindMin(self, conquered);
        if (Distance(v) == INFINITY)
            break;

        conquered[tracker++] = v;
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

    GraphResult r = SetDistance(self->V[start], 0, NULL);
    if (r != Graph_Success)
        return r;

    PriorityQueue* pq = InitPQ(self);
    if (pq == NULL)
        return Graph_DependencyError;

    while (!PQ_IsEmpty(pq)) {
        Vertex* v = PQ_Remove(pq);

        // unreachable vertices, time to bail
        if (v->data == NULL)
            break;

        Edge* e = v->edges;
        while (e != NULL) {
            double dist = Distance(v) + e->weight;
            Vertex* u = self->V[e->head];
            SetDistance(u, dist, v);

            // We only need to re prioritize if the distance changed
            if (dist == Distance(u))
                PQ_Reprioritize(pq, u);

            e = e->next;
        }
    }

    PQ_Destroy(pq, NULL);
    return Graph_Success;
}
