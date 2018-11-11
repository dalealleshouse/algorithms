#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "MemAllocMock.h"
#include "graph.h"

const size_t INITIAL_ALLOC = 100;
const size_t REALLOC_FACTOR = 2;
const static size_t BUFFER_SIZE = 1024;

Graph* Graph_Create() { return calloc(sizeof(Graph), 1); }

static size_t _NextAllocSize(unsigned id)
{
    if (id < INITIAL_ALLOC)
        return INITIAL_ALLOC;

    return id * REALLOC_FACTOR;
}

GraphResult Graph_AddVertex(Graph* self, const unsigned id)
{
    if (self == NULL)
        return Graph_NullParameter;

    if (self->n_allocated <= id) {
        size_t next_size = _NextAllocSize(id);
        vertex* temp = realloc(self->V, sizeof(vertex) * next_size);

        if (temp == NULL)
            return Graph_FailedMemoryAllocation;

        self->V = temp;
        memset(&self->V[self->n_allocated], 0,
            sizeof(self->V[0]) * (next_size - self->n_allocated));
        self->n_allocated = next_size;
    }

    if (self->V[id].initalized == true)
        return Graph_DuplicateVertex;

    self->n++;
    self->V[id].vertex_id = id;
    self->V[id].degree = 0;
    self->V[id].initalized = true;

    return Graph_Success;
}

GraphResult Graph_AddEdge(Graph* self, const unsigned tail, const unsigned head)
{
    if (self == NULL)
        return Graph_NullParameter;

    if (tail == head)
        return Graph_EdgeIsSelfLoop;

    if (tail >= self->n_allocated || head >= self->n_allocated
        || self->V[tail].initalized == false
        || self->V[head].initalized == false)
        return Graph_InvalidVertex;

    self->m++;

    if (self->m_allocated < self->m) {
        size_t next_size = _NextAllocSize(self->m);
        edge* temp = realloc(self->E, sizeof(edge) * next_size);

        if (temp == NULL)
            return Graph_FailedMemoryAllocation;

        self->E = temp;
        self->m_allocated = next_size;
    }

    edge* e = &self->E[self->m - 1];

    e->tail = tail;
    e->head = head;
    self->V[tail].degree++;
    self->V[head].degree++;

    return Graph_Success;
}

static void _ShiftEdges(edge* edges, size_t size, size_t edge_index)
{
    // size = 4
    // if index = 0 shift = 4 - 0 - 1 = 3
    // if index = 1 shift = 4 - 1 - 1 = 2
    // if index = 2 shift = 4 - 2 - 1 = 1
    // if index = 3 shift = 4 - 3 - 1 = 0
    // [0][1][2][3]
    memmove(&edges[edge_index], &edges[edge_index + 1],
        sizeof(edge) * (size - edge_index - 1));
}

static void _DeleteEdge(Graph* self, const size_t edge_index)
{
    self->V[self->E[edge_index].head].degree--;
    self->V[self->E[edge_index].tail].degree--;
    _ShiftEdges(self->E, self->m, edge_index);
    self->m--;
}

static void _DeleteVertex(Graph* self, const unsigned id)
{
    self->V[id].initalized = false;
    self->n--;
}

GraphResult Graph_CollapseEdge(Graph* self, size_t edge_index)
{
    if (self == NULL)
        return Graph_NullParameter;

    if (edge_index >= self->m)
        return Graph_InvalidEdgeIndex;

    edge* edge = &self->E[edge_index];
    vertex* tail = &self->V[edge->tail];
    vertex* head = &self->V[edge->head];

    _DeleteVertex(self, edge->tail);
    _DeleteEdge(self, edge_index);

    for (size_t i = 0; i < self->m; i++) {
        if (self->E[i].tail == tail->vertex_id) {
            self->E[i].tail = head->vertex_id;
            head->degree++;
        }

        if (self->E[i].head == tail->vertex_id) {
            self->E[i].head = head->vertex_id;
            head->degree++;
        }

        if (self->E[i].head == self->E[i].tail) {
            _DeleteEdge(self, i);
            i -= 1;
        }
    }

    return Graph_Success;
}

static void _AddVertexIfMissing(Graph* self, unsigned vertex)
{
    if (vertex > self->n_allocated || !self->V[vertex].initalized)
        Graph_AddVertex(self, vertex);
}

Graph* Graph_FromFile(const char* path)
{
    if (path == NULL)
        return NULL;

    if (access(path, R_OK) != 0)
        return NULL;

    FILE* file = fopen(path, "r");
    if (file == NULL)
        return NULL;

    Graph* self = Graph_Create();
    if (self == NULL) {
        fclose(file);
        return NULL;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, file)) {
        char* remaining;
        unsigned vertex = strtoul(line, &remaining, 10);

        if (vertex == 0)
            continue;

        _AddVertexIfMissing(self, vertex);

        unsigned edge = strtoul(remaining, &remaining, 10);
        while (edge > 0) {
            _AddVertexIfMissing(self, edge);

            if (edge > vertex)
                Graph_AddEdge(self, vertex, edge);

            edge = strtoul(remaining, &remaining, 10);
        }
    }

    fclose(file);

    return self;
}

void Graph_Print(const Graph* self, FILE* file)
{
    for (size_t i = 0; i < self->n_allocated; i++) {
        if (!self->V[i].initalized)
            continue;

        vertex* v = &self->V[i];
        fprintf(file, "Vertex %u, degree=%zu\n", v->vertex_id, v->degree);
    }

    for (size_t i = 0; i < self->m; i++) {
        fprintf(file, "%zu = tail %u, head=%u\n", i, self->E[i].tail,
            self->E[i].head);
    }
}

Graph* Graph_Clone(const Graph* source)
{
    if (source == NULL)
        return NULL;

    Graph* clone = Graph_Create();
    if (clone == NULL)
        return NULL;

    clone->m = source->m;
    clone->n = source->n;
    clone->m_allocated = source->m_allocated;
    clone->n_allocated = source->n_allocated;

    if (source->V != NULL) {
        size_t v_size = sizeof(vertex) * source->n_allocated;
        clone->V = calloc(v_size, 1);

        if (clone->V != NULL)
            memcpy(clone->V, source->V, v_size);
    }

    if (source->E != NULL) {
        size_t e_size = sizeof(edge) * source->m_allocated;
        clone->E = calloc(e_size, 1);

        if (clone->E != NULL)
            memcpy(clone->E, source->E, e_size);
    }

    return clone;
}

void Graph_Destroy(Graph* self)
{
    if (self == NULL)
        return;

    free(self->V);
    free(self->E);
    free(self);
}
