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
    strategy->conqueror(self->V[vertex_id]);

    while (!Queue_IsEmpty(q)) {
        Vertex* v = Queue_Dequeue(q);
        Edge* e = v->edges;
        while (e != NULL) {
            Vertex* x = self->V[e->head];
            if (!strategy->is_conquered(x)) {
                strategy->conqueror(x);
                Queue_Enqueue(q, x);
            }
            e = e->next;
        }
    }

    Queue_Destroy(q);
    return Graph_Success;
}
