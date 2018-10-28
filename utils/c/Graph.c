#include <stdlib.h>

#include "Graph.h"
#include "MemAllocMock.h"

// We may need a better way to log errors, but this will work for now
#define GRAPH_ERROR(result)                                                    \
    {                                                                          \
        fprintf(stderr, "Graph Error: %s, %s, %s, %d\n",                       \
            Graph_ErrorMessage(result), __FILE__, __FUNCTION__, __LINE__);     \
    }

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

    v->degree = 0;
    v->edges = NULL;
    v->id = id;
    v->data = data;

    return v;
}

void Graph_Destroy(Graph* self, freer freer)
{
    if (self == NULL)
        return;

    free(self);
}

void Graph_VertexDestroy(Vertex* vertex, freer freer)
{
    if (freer != NULL)
        freer(vertex->data);
    else
        free(vertex->data);

    free(vertex);
}

char* Graph_ErrorMessage(GraphResult result)
{
    switch (result) {
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
