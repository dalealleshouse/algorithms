#pragma once

#include "result_code.h"

/*
 * Simple structure to represent a weighted vertex without edges (edges are
 * implicitly defined by <PathGraph>
 */
typedef struct WeightedVertex {
  unsigned int id;
  unsigned long weight;
} WeightedVertex;

Result WeightedVertex_Init(unsigned int id, unsigned long weight,
                           WeightedVertex** vw);
void WeightedVertex_Destory(WeightedVertex* vw);

/*
 * A set of vertices that comprise a Path Graph:
 *
 * Vertex 1 -> Vertex 2 -> Vertex 3 -> etc...
 *
 * Edges implicitly exist between neighboring vertices
 */
typedef struct PathGraph {
  size_t n;
  WeightedVertex** vertices;
} PathGraph;

Result PathGraph_Create(unsigned int ids[], unsigned long weights[], size_t n,
                        PathGraph** graph);
void PathGraph_Destroy(PathGraph*);

/*
 * Defines a weighted independent set
 */
typedef struct WeightedIndependentSet {
  size_t n;
  unsigned long weight;
  WeightedVertex** vertices;
} WeightedIndependentSet;

void WeightedIndependentSet_Destroy(WeightedIndependentSet* wis);

/*
 * Common representation for the empty set
 */
extern const WeightedIndependentSet EMPTY_SET;

Result WeightedIndependentSet_Recursive(PathGraph* graph,
                                        WeightedIndependentSet** conclusion);

Result WeightedIndependentSet_Dynamic(PathGraph* graph,
                                      WeightedIndependentSet** conclusion);

Result WeightedIndependentSet_Dynamic_Reconstruction(
    PathGraph* graph, WeightedIndependentSet** conclusion);
