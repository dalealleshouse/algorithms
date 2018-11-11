#include <stdlib.h>
#include <unistd.h>

#include "Graph.h"
#include "MemAllocMock.h"

const static size_t BUFFER_SIZE = 1024;

static void EdgeDestroy(Edge* edge) { free(edge); }

static void EdgesDestory(Edge* edge)
{
    Edge* curr = edge;
    while (curr != NULL) {
        Edge* temp = curr->next;
        EdgeDestroy(curr);
        curr = temp;
    }
}

static void VertexDestroy(Vertex* vertex, freer freer)
{
    if (vertex == NULL)
        return;

    if (freer != NULL)
        freer(vertex->data);

    EdgesDestory(vertex->edges);
    EdgesDestory(vertex->in_edges);

    free(vertex);
}

/*
 * This function is a bit over-engineered for a private function, but it was
 * orginally meant to be public. It was eaiser to just create all the vertices
 * during create than try to add them in after the fact and keep track of nulls
 * and such.
 */
static GraphResult Graph_AddVertex(Graph* self, int id, void* data)
{
    if (self == NULL)
        return Graph_NullParameter;

    if (id < 0)
        return Graph_InvalidVertexId;

    // it's ok to cast to size_t b/c the line above ensures it isn't negative
    if ((size_t)id >= self->n)
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
    v->in_edges = NULL;

    self->V[id] = v;

    return Graph_Success;
}

static void AddVertex_PrintError(Graph* self, int id)
{
    GraphResult result = Graph_AddVertex(self, id, NULL);
    if (result != Graph_Success)
        GRAPH_ERROR(result);
}

static void AddEdge_PrintError(Graph* self, int head, int tail)
{
    GraphResult result = Graph_AddEdge(self, head, tail);
    if (result != Graph_Success)
        GRAPH_ERROR(result);
}

Graph* Graph_Create(size_t n)
{
    Graph* graph = malloc(sizeof(Graph));

    if (graph == NULL) {
        GRAPH_ERROR(Graph_FailedMemoryAllocation);
        return NULL;
    }

    graph->V = calloc(sizeof(Vertex*), n);
    graph->m = 0;
    graph->n = n;

    for (size_t i = 0; i < n; i++)
        AddVertex_PrintError(graph, i);

    return graph;
}

static Edge* Graph_EdgeInit(int head, int tail)
{
    Edge* edge = calloc(sizeof(Edge), 1);
    if (edge == NULL)
        return NULL;

    edge->head = head;
    edge->tail = tail;

    return edge;
}

// It is assumed that all paramters are validated by the caller
static GraphResult Graph_AddInEdge(Graph* self, int head, int tail)
{
    Edge* edge = Graph_EdgeInit(head, tail);
    if (edge == NULL)
        return Graph_FailedMemoryAllocation;

    edge->next = self->V[head]->in_edges;
    self->V[head]->in_edges = edge;

    return Graph_Success;
}

// It is assumed that all paramters are validated by the caller
static GraphResult Graph_AddOutEdge(Graph* self, int head, int tail)
{
    Edge* edge = Graph_EdgeInit(head, tail);
    if (edge == NULL)
        return Graph_FailedMemoryAllocation;

    edge->next = self->V[tail]->edges;
    self->V[tail]->edges = edge;

    return Graph_Success;
}

GraphResult Graph_AddEdge(Graph* self, int head, int tail)
{
    if (self == NULL)
        return Graph_NullParameter;

    if (head < 0 || tail < 0)
        return Graph_InvalidVertexId;

    // cast to size_t ok b/c the previous condition ensures it's not negative
    if ((size_t)head >= self->n || (size_t)tail >= self->n)
        return Graph_VertexIdExceedsMaxSize;

    if (self->V[head] == NULL || self->V[tail] == NULL)
        return Graph_InvalidVertexId;

    GraphResult result;

    result = Graph_AddInEdge(self, head, tail);
    if (result != Graph_Success)
        return result;

    result = Graph_AddOutEdge(self, head, tail);
    if (result != Graph_Success)
        return result;

    self->m++;

    return Graph_Success;
}

Graph* Graph_FromFile(const size_t n, const char* path)
{
    if (path == NULL) {
        GRAPH_ERROR(Graph_NullParameter);
        return NULL;
    }

    if (access(path, R_OK) != 0) {
        GRAPH_ERROR(Graph_InvalidFilePath);
        return NULL;
    }

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        return NULL;
    }

    Graph* g = Graph_Create(n);
    if (g == NULL) {
        GRAPH_ERROR(Graph_FailedMemoryAllocation);
        fclose(file);
        return NULL;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, file)) {
        char* remaining;
        int vertex = strtoul(line, &remaining, 10);

        int edge = strtoul(remaining, &remaining, 10);
        while (edge != 0) {
            AddEdge_PrintError(g, edge, vertex);
            edge = strtoul(remaining, &remaining, 10);
        }
    }

    fclose(file);
    return g;
}

void Graph_Destroy(Graph* self, freer freer)
{
    if (self == NULL)
        return;

    for (size_t i = 0; i < self->n; i++)
        VertexDestroy(self->V[i], freer);

    free(self->V);
    free(self);
}

char* Graph_ErrorMessage(GraphResult result)
{
    switch (result) {
    case Graph_FileOpenError:
        return "fopen returned NULL";
    case Graph_InvalidFilePath:
        return "Invalid file path";
    case Graph_VertexIdExceedsMaxSize:
        return "The ID of the vertex is greater than max size";
    case Graph_DuplicateVertexId:
        return "Attempt to add two vertices with the same id";
    case Graph_InvalidVertexId:
        return "Invalid vertex ID";
    case Graph_FailedMemoryAllocation:
        return "Failed to allocate memory";
    case Graph_NullParameter:
        return "One of the requited parameters passed to the function is NULL";
    case Graph_Success:
        return "Success";
    default:
        return "Unknown error code";
    }
}
