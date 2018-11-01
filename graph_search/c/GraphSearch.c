#include <stdlib.h>

#include "GraphSearch.h"
#include "Queue.h"

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

    VertexData* d = malloc(sizeof(VertexData));
    d->am_i_conquered = true;
    d->shortest_path = 0;
    v->data = d;

    return true;
}

static bool is_conquered(Vertex* v)
{
    if (v->data == NULL)
        return false;

    return ((VertexData*)v->data)->am_i_conquered;
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

    VertexData* d = malloc(sizeof(VertexData));
    d->am_i_conquered = true;
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
