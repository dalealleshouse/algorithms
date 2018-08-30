#pragma once

#include <stddef.h>

typedef struct {
    unsigned node_id;
} vertex;

typedef struct {
    vertex* tail;
    vertex* head;
} edge;

typedef struct {
    vertex* verticies;
    size_t vertex_count;
    edge* edges;
    size_t edge_count;
} graph;

int min_cut(graph* graph);
