#include <stdlib.h>

#include "GraphSearch.h"
#include "Queue.h"

static bool is_conquered(Vertex* v)
{
    if (v->data == NULL)
        return false;

    return ((VertexData*)v->data)->am_i_conquered;
}

static VertexData* VertexData_Create(size_t shortest_path, int component_id)
{
    VertexData* d = calloc(sizeof(VertexData), 1);

    if (d == NULL) {
        GRAPH_ERROR(Graph_FailedMemoryAllocation);
        return NULL;
    }

    d->am_i_conquered = true;
    d->shortest_path = shortest_path;
    d->component_id = component_id;

    return d;
}

GraphResult Graph_BFS(Graph* self, int vertex_id, SearchStrategy* strategy)
{
    if (self == NULL || strategy == NULL || strategy->conqueror == NULL
        || strategy->is_conquered == NULL)
        return Graph_NullParameter;

    if (vertex_id < 0)
        return Graph_InvalidVertexId;

    if ((size_t)vertex_id >= self->max_size)
        return Graph_VertexIdExceedsMaxSize;

    Queue* q = Queue_Create();
    Queue_Enqueue(q, self->V[vertex_id]);
    strategy->conqueror(self->V[vertex_id], NULL);

    while (!Queue_IsEmpty(q)) {
        Vertex* v = Queue_Dequeue(q);
        Edge* e = v->edges;
        while (e != NULL) {
            Vertex* w = self->V[e->head];
            if (!strategy->is_conquered(w)) {
                strategy->conqueror(w, v);
                Queue_Enqueue(q, w);
            }
            e = e->next;
        }
    }

    Queue_Destroy(q);
    return Graph_Success;
}

static bool reachable_conqueror(Vertex* v, Vertex* p)
{
    (void)p;
    if (v->data != NULL)
        return true;

    v->data = VertexData_Create(0, 0);

    return true;
}

GraphResult Graph_Reachable(Graph* self, int vertex_id)
{
    SearchStrategy strategy = { reachable_conqueror, is_conquered };
    return Graph_BFS(self, vertex_id, &strategy);
}

static bool shortest_path_conqueror(Vertex* v, Vertex* p)
{
    if (v == NULL || v->data != NULL)
        return false;

    VertexData* d = VertexData_Create(0, 0);
    if (p == NULL)
        d->shortest_path = 0;
    else
        d->shortest_path = ((VertexData*)p->data)->shortest_path + 1;

    v->data = d;

    return true;
}

GraphResult Graph_ShortestPath(Graph* self, int vertex_id)
{
    SearchStrategy strategy = { shortest_path_conqueror, is_conquered };
    return Graph_BFS(self, vertex_id, &strategy);
}

static int component_id = 0;
static bool connected_conqueror(Vertex* v, Vertex* p)
{
    (void)p;
    if (v == NULL || v->data != NULL)
        return false;

    v->data = VertexData_Create(0, component_id);
    return true;
}

GraphResult Graph_Connected(Graph* self)
{
    SearchStrategy strategy = { connected_conqueror, is_conquered };
    component_id = 0;
    for (size_t i = 0; i < self->max_size; i++) {
        if (self->V[i] != NULL && !is_conquered(self->V[i])) {
            component_id++;
            GraphResult result = Graph_BFS(self, i, &strategy);

            if (result != Graph_Success)
                return result;
        }
    }

    return Graph_Success;
}
