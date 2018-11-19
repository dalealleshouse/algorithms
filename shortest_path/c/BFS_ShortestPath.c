#include <stdlib.h>

#include "BFS_ShortestPath.h"

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

static bool is_conquered(Vertex* v)
{
    if (v->data == NULL)
        return false;

    return ((VertexData*)v->data)->am_i_conquered;
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
