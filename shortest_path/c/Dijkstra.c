#include <stdlib.h>
#include <string.h>

#include "Dijkstra.h"
#include "include/MemAllocMock.h"

static GraphResult SetDistance(Vertex* v, size_t length)
{
    if (v == NULL)
        return Graph_NullParameter;

    if (v->data == NULL) {
        v->data = calloc(sizeof(ShortestPath), 1);

        if (v->data == NULL)
            return Graph_FailedMemoryAllocation;
    }

    ((ShortestPath*)v->data)->distance = length;
    return Graph_Success;
}

/* static size_t Distance(Vertex* v) */
/* { */
/*     if (v == NULL || v->data == NULL) */
/*         return 0; */

/*     return ((ShortestPath*)v->data)->distance; */
/* } */

/* static Edge* CheapestEdge(Edge* e) */
/* { */
/*     if (e == NULL) */
/*         return NULL; */

/*     Edge* cheapest = e; */
/*     while (e != NULL) { */
/*         if (e->weight < cheapest->weight) */
/*             cheapest = e; */
/*         e = e->next; */
/*     } */

/*     return cheapest; */
/* } */

GraphResult Graph_DijkstraShortestPath(Graph* self, int start)
{
    if (self == NULL)
        return Graph_NullParameter;

    if (start < 0 || (size_t)start >= self->n)
        return Graph_InvalidVertexId;

    Vertex* Q[self->n];
    memcpy(Q, self->V, sizeof(Vertex*) * self->n);

    GraphResult r = SetDistance(self->V[start], 0);
    if (r != Graph_Success)
        return r;

    /* Edge* e = CheapestEdge(self->V[start]->edges); */

    /* while (e != NULL) { */
    /*     SetWeight(self->V[e->head], e->weight + Distance(self->V[e->tail])); */
    /*     e = CheapestEdge(self->V[e->head]->edges); */
    /* } */

    return Graph_Success;
}
