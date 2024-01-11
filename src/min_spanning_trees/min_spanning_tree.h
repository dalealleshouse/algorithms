#pragma once

#include "graph.h"
#include "result_code.h"

typedef struct {
  // First edge in the linked list
  Edge* edge_head;

  // Last edge in the linked list
  Edge* edge_tail;

  // Number of edges in the linked list
  size_t edge_count;

  // Sum of the weight from each included edge
  int cost;
} MinSpanTree;

// Finds the min spanning tree (MST) of the input graph
// Assumptions:
//  graph is connected and directed
//
// Parameters:
//  graph: input graph
//  mst: output parameter that will populated with mst value
//
// returns:
//  Result code
//
// Consumer is responsible for deallocating edges in the MinSpanTree struct
Result PrimMinSpanTreeNaive(const Graph* graph, MinSpanTree* mst);
Result PrimMinSpanTree(const Graph* graph, MinSpanTree* mst);
Result KruskalMinSpanTree(const Graph* graph, MinSpanTree* mst);

// Creates an empty min span tree struct
MinSpanTree* MinSpanTree_Create();

// Frees memory allocated by PrimMinSpanTree functions
void MinSpanTree_Destroy(MinSpanTree* mst);
void MinSpanTree_Edges_Destroy(Edge* edges);
