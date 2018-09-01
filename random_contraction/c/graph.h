#pragma once

#include <stddef.h>

typedef struct {
    size_t n;
    size_t m;
} adj_list;

int graph_init(adj_list* graph);

int graph_destroy(adj_list* graph);
