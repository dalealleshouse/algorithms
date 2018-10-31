#include <stdlib.h>

#include "Graph.h"
#include "MemAllocMock.h"

// We may need a better way to log errors, but this will work for now
#define GRAPH_ERROR(result)                                                    \
    {                                                                          \
        fprintf(stderr, "Graph Error: %s, %s, %s, %d\n",                       \
            Graph_ErrorMessage(result), __FILE__, __FUNCTION__, __LINE__);     \
    }

static void EdgeDestroy(Edge* edge) { free(edge); }

static void VertexDestroy(Vertex* vertex, freer freer)
{
    if (vertex == NULL)
        return;

    if (freer != NULL)
        freer(vertex->data);

    Edge* curr = vertex->edges;
    while (curr != NULL) {
        Edge* temp = curr->next;
        EdgeDestroy(curr);
        curr = temp;
    }

    free(vertex);
}

static Edge* Graph_EdgeCreate(int head)
{
    Edge* edge = malloc(sizeof(Edge));
    if (edge == NULL) {
        GRAPH_ERROR(Graph_FailedMemoryAllocation);
        return NULL;
    }

    edge->head = head;
    edge->next = NULL;

    return edge;
}

Graph* Graph_Create(size_t max_size)
{
    Graph* graph = malloc(sizeof(Graph));

    if (graph == NULL) {
        GRAPH_ERROR(Graph_FailedMemoryAllocation);
        return NULL;
    }

    graph->V = calloc(sizeof(Vertex*), max_size);
    graph->m = 0;
    graph->n = 0;
    graph->max_size = max_size;

    return graph;
}

GraphResult Graph_AddVertex(Graph* self, int id, void* data)
{
    if (self == NULL)
        return Graph_NullParameter;

    if (id < 0)
        return Graph_InvalidVertexId;

    // it's ok to cast to size_t b/c the line above ensures it isn't negative
    if ((size_t)id >= self->max_size)
        return Graph_VertexIdExceedsMaxSize;

    if (self->V[id] != NULL)
        return Graph_DuplicateVertexId;

    Vertex* v = malloc(sizeof(Vertex));
    if (v == NULL)
        return Graph_FailedMemoryAllocation;

    v->id = id;
    v->data = data;
    v->degree = 0;
    v->edges = NULL;

    self->V[id] = v;
    self->n++;

    return Graph_Success;
}

GraphResult Graph_AddEdge(Graph* self, int head, int tail)
{
    if (self == NULL)
        return Graph_NullParameter;

    if (head < 0 || tail < 0)
        return Graph_InvalidVertexId;

    // cast to size_t ok b/c the previous condition ensures it's not negative
    if ((size_t)head >= self->max_size || (size_t)tail >= self->max_size)
        return Graph_VertexIdExceedsMaxSize;

    if (self->V[head] == NULL || self->V[tail] == NULL)
        return Graph_InvalidVertexId;

    Edge* edge = malloc(sizeof(Edge));
    if (edge == NULL)
        return Graph_FailedMemoryAllocation;

    edge->head = head;
    edge->next = self->V[tail]->edges;
    self->V[tail]->edges = edge;
    self->m++;

    return Graph_Success;
}

void Graph_Destroy(Graph* self, freer freer)
{
    if (self == NULL)
        return;

    for (size_t i = 0; i < self->max_size; i++)
        VertexDestroy(self->V[i], freer);

    free(self->V);
    free(self);
}

char* Graph_ErrorMessage(GraphResult result)
{
    switch (result) {
    case Graph_VertexIdExceedsMaxSize:
        return "The ID of the vertex is greater than max size";
    case Graph_DuplicateVertexId:
        return "Attempt to add two vertices with the sam id";
    case Graph_InvalidVertexId:
        return "Invalid vertex ID";
    case Graph_FailedMemoryAllocation:
        return "Failed to allocate memory";
    case Graph_NullParameter:
        return "One of the requited parameters passed to the fuction is NULL";
    case Graph_Success:
        return "Success";
    default:
        return "Unknown error code";
    }
}
