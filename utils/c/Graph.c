#include <stdlib.h>

#include "Graph.h"
#include "MemAllocMock.h"

// We may need a better way to log errors, but this will work for now
#define GRAPH_ERROR(result)                                                    \
    {                                                                          \
        fprintf(stderr, "Graph Error: %s, %s, %s, %d\n",                       \
            Graph_ErrorMessage(result), __FILE__, __FUNCTION__, __LINE__);     \
    }

const size_t INIT_VERTEX_SIZE = 100;
const size_t GROWTH_FACTOR = 2;

GraphResult Graph_Init(Graph* self)
{
    if (self == NULL)
        return Graph_NullParameter;

    self->m = 0;
    self->n = 0;
    self->V = NULL;

    return Graph_Success;
}

Graph* Graph_Create(void)
{
    Graph* graph = malloc(sizeof(Graph));

    if (graph == NULL) {
        GRAPH_ERROR(Graph_FailedMemoryAllocation);
        return NULL;
    }

    Graph_Init(graph);
    return graph;
}

GraphResult Graph_VertexInit(Vertex* vertex)
{
    if (vertex == NULL)
        return Graph_NullParameter;

    vertex->id = -1;
    vertex->degree = 0;
    vertex->edges = NULL;
    vertex->data = NULL;

    return Graph_Success;
}

Vertex* Graph_VertexCreate(int id, void* data)
{
    Vertex* v = malloc(sizeof(Vertex));

    if (v == NULL) {
        GRAPH_ERROR(Graph_FailedMemoryAllocation);
        return NULL;
    }

    Graph_VertexInit(v);
    v->id = id;
    v->data = data;

    return v;
}

GraphResult Graph_EdgeInit(Edge* edge)
{
    if (edge == NULL)
        return Graph_NullParameter;

    edge->head = -1;
    edge->next = NULL;

    return Graph_Success;
}

Edge* Graph_EdgeCreate(int head)
{
    Edge* edge = malloc(sizeof(Edge));
    if (edge == NULL) {
        GRAPH_ERROR(Graph_FailedMemoryAllocation);
        return NULL;
    }

    Graph_EdgeInit(edge);
    edge->head = head;

    return edge;
}

GraphResult Graph_AddVertex(Graph* self, Vertex* vertex)
{
    if (self == NULL || vertex == NULL)
        return Graph_NullParameter;

    if (vertex->id < 0)
        return Graph_InvalidVertexId;

    if (self->V == NULL)
        self->V = calloc(sizeof(Vertex*), 100);

    if (self->V[vertex->id] != NULL)
        return Graph_DuplicateVertexId;

    self->V[vertex->id] = vertex;

    self->n++;
    return Graph_Success;
}

void Graph_Destroy(Graph* self, freer freer)
{
    if (self == NULL)
        return;

    for (size_t i = 0; i < self->n; i++)
        Graph_VertexDestroy(self->V[i], freer);

    free(self);
}

void Graph_VertexDestroy(Vertex* vertex, freer freer)
{
    if (vertex == NULL)
        return;

    if (freer != NULL)
        freer(vertex->data);
    else
        free(vertex->data);

    free(vertex);
}

void Graph_EdgeDestroy(Edge* edge) { free(edge); }

char* Graph_ErrorMessage(GraphResult result)
{
    switch (result) {
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
