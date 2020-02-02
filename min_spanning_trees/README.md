# Minimum Spanning Trees (MTS)
Prerequisite: [Graph Concepts](../graph_concepts)

MTS = Subset of edges that satisfy two properties
1. Does not contain a cycle
1. A path exists between every pair of vertices

By definition, a MTS has ![n-1](https://latex.codecogs.com/gif.latex?n-1) edges

Input = Undirected graph with each edge having a real-valued cost

Goal = Compute a spanning tree of the graph with the minimum possible sum of
edge costs

Assumptions:
- The input graph is connected

## Prim's Algorithm
Named after Robert C. Prim who discovered the algorithm in 1957. However, it was
discovered that Vojtěch Jarník discovered it in 1930, so it is sometimes know as
the Prim-Jarník algorithm.

### Asymptotic Time Complexity
![O((m+n)\log
n))](https://latex.codecogs.com/gif.latex?O((m&plus;n)\log&space;n)))


## Kruskal's Algorithm
