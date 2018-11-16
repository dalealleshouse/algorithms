#include <stdlib.h>

#include "include/Queue.h"
#include "GraphSearch.h"

static bool is_conquered(Vertex* v)
{
    if (v->data == NULL)
        return false;

    return ((VertexData*)v->data)->am_i_conquered;
}

static VertexData* VertexData_Create(int value)
{
    VertexData* d = calloc(sizeof(VertexData), 1);

    if (d == NULL) {
        GRAPH_ERROR(Graph_FailedMemoryAllocation);
        return NULL;
    }

    d->am_i_conquered = true;
    d->value = value;

    return d;
}

static GraphResult SearchIsValid(
    Graph* self, int vertex_id, SearchStrategy* strategy)
{
    if (self == NULL || strategy == NULL || strategy->conqueror == NULL
        || strategy->is_conquered == NULL)
        return Graph_NullParameter;

    if (vertex_id < 0)
        return Graph_InvalidVertexId;

    if ((size_t)vertex_id >= self->n)
        return Graph_VertexIdExceedsMaxSize;

    return Graph_Success;
}

GraphResult Graph_BFS(Graph* self, int vertex_id, SearchStrategy* strategy)
{
    GraphResult result = SearchIsValid(self, vertex_id, strategy);
    if (result != Graph_Success)
        return result;

    Queue* q = Queue_Create();
    Queue_Enqueue(q, self->V[vertex_id]);
    bool con_result = strategy->conqueror(self->V[vertex_id], NULL);
    if (!con_result)
        return Graph_Success;

    while (!Queue_IsEmpty(q)) {
        Vertex* v = Queue_Dequeue(q);
        Edge* e = v->edges;
        while (e != NULL) {
            Vertex* w = self->V[e->head];
            if (!strategy->is_conquered(w)) {
                con_result = strategy->conqueror(w, v);
                if (!con_result) {
                    Queue_Destroy(q);
                    return Graph_Success;
                }

                Queue_Enqueue(q, w);
            }
            e = e->next;
        }
    }

    Queue_Destroy(q);
    return result;
}

GraphResult Graph_DFS(Graph* self, int vertex_id, SearchStrategy* strategy)
{
    GraphResult result = SearchIsValid(self, vertex_id, strategy);
    if (result != Graph_Success)
        return result;

    Stack* s = Stack_Create();
    Stack_Push(s, self->V[vertex_id]);

    Vertex* prev = NULL;
    while (!Stack_IsEmpty(s)) {
        Vertex* v = Stack_Pop(s);
        if (!strategy->is_conquered(v)) {
            bool con_result = strategy->conqueror(v, prev);
            if (!con_result) {
                Stack_Destroy(s);
                return Graph_Success;
            }

            Edge* e = v->edges;
            while (e != NULL) {
                Vertex* w = self->V[e->head];
                Stack_Push(s, w);
                e = e->next;
            }
            prev = v;
        }
    }

    Stack_Destroy(s);
    return result;
}

static bool reachable_conqueror(Vertex* v, Vertex* p)
{
    (void)p;
    if (v->data != NULL)
        return true;

    v->data = VertexData_Create(0);

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

    int shortest_path;
    if (p == NULL)
        shortest_path = 0;
    else
        shortest_path = ((VertexData*)p->data)->value + 1;

    VertexData* d = VertexData_Create(shortest_path);
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

    v->data = VertexData_Create(component_id);
    return true;
}

GraphResult Graph_Connected(Graph* self)
{
    SearchStrategy strategy = { connected_conqueror, is_conquered };
    component_id = 0;
    for (size_t i = 0; i < self->n; i++) {
        if (self->V[i] != NULL && !is_conquered(self->V[i])) {
            component_id++;
            GraphResult result = Graph_BFS(self, i, &strategy);

            if (result != Graph_Success)
                return result;
        }
    }

    return Graph_Success;
}

// sooooooooo tread unsafe it hurts
static int order = 0;
static void Graph_DFS_TopSort(Graph* self, int vertex_id)
{
    Vertex* v = self->V[vertex_id];
    reachable_conqueror(v, NULL);

    Edge* e = v->edges;
    while (e != NULL) {
        Vertex* w = self->V[e->head];
        if (!is_conquered(w))
            Graph_DFS_TopSort(self, e->head);

        e = e->next;
    }

    ((VertexData*)v->data)->value = order;
    order--;
}

GraphResult Graph_TopSort(Graph* self)
{
    if (self == NULL)
        return Graph_NullParameter;

    order = self->n;

    for (size_t i = 0; i < self->n; i++) {
        if (!is_conquered(self->V[i]))
            Graph_DFS_TopSort(self, i);
    }

    return Graph_Success;
}

static bool magic_ordering_conqueror(Vertex* v, Vertex* p)
{
    (void)p;
    if (v->data != NULL)
        return true;

    v->data = VertexData_Create(-1);

    return true;
}

static int scc_id = -1;
static bool scc_conqueror(Vertex* v, Vertex* p)
{
    (void)p;
    if (v->data == NULL)
        return true;

    ((VertexData*)v->data)->value = scc_id;
    return true;
}

static bool scc_is_conquered(Vertex* v)
{
    if (v->data == NULL)
        return false;

    return ((VertexData*)v->data)->value != -1;
}

/*
 * Reverse topological ordering
 * This will generate a valid order for discovering strongly connected
 * components
 */
static Stack* Graph_MagicOrdering(Graph* self, int vertex_id, Stack* order)
{
    Vertex* v = self->V[vertex_id];
    magic_ordering_conqueror(v, NULL);

    Edge* e = v->in_edges;
    while (e != NULL) {
        Vertex* w = self->V[e->tail];
        if (!is_conquered(w))
            order = Graph_MagicOrdering(self, e->tail, order);

        e = e->next;
    }

    Stack_Push(order, v);
    return order;
}

Stack* Graph_SCC_MagicOrdering(Graph* self)
{
    if (self == NULL) {
        GRAPH_ERROR(Graph_NullParameter);
        return NULL;
    }

    Stack* ordering = Stack_Create();

    for (size_t i = 0; i < self->n; i++) {
        if (!is_conquered(self->V[i]))
            Graph_MagicOrdering(self, i, ordering);
    }

    return ordering;
}

GraphResult Graph_SCC(Graph* self)
{
    if (self == NULL)
        return Graph_NullParameter;

    Stack* mo = Graph_SCC_MagicOrdering(self);

    for (size_t i = 0; i < self->n; i++) {
        Vertex* v = Stack_Pop(mo);
        if (scc_is_conquered(v))
            continue;

        scc_id = v->id;
        SearchStrategy scc = { scc_conqueror, scc_is_conquered };

        Graph_DFS(self, v->id, &scc);
    }

    Stack_Destroy(mo);
    return Graph_Success;
}
