#include "Graph.h"

typedef struct GraphPrivate {
    // The number of vertices that can be accommodated before reallocation.
    // This starts at 100 and doubles on every time more space is required
    size_t n_allocated;

    // The number of edges that can be accommodated before reallocation.
    // This starts at 100 and doubles on every time more space is required
    size_t m_allocated;
} GraphPrivate;

typedef struct VertexPrivate {
    // Indicates if the vertex is initialized and valid
    bool initalized;
} VertexPrivate;
