#pragma once

#include <stddef.h>

typedef struct edge {
    struct vertex* tail;
    struct vertex* head;
} edge;

typedef struct vertex {
    unsigned node_id;
    size_t edge_count;
    edge* edges;
} vertex;


typedef struct {
    vertex* verticies;
    size_t vertex_count;
    edge* edges;
    size_t edge_count;
} graph;

int collapse_edge(graph* graph, size_t edge_index);
int min_cut(graph* graph);
