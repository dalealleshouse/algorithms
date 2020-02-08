# Minimum Spanning Trees (MST)
Prerequisite: [Graph Concepts](../graph_concepts)

A Minimum Spanning Tree (MST) is used to connect points together as cheaply as
possible. For instance, servers on a network or documents on a web page.

MST = Subset of edges that satisfy two properties
1. Does not contain a cycle
1. A path exists between every pair of vertices

By definition, a MTS has ![n-1](https://latex.codecogs.com/gif.latex?n-1) edges

Input = Undirected graph with each edge having a real-valued cost

Goal = Compute a MST of the graph with the minimum possible sum of edge costs

Assumptions:
- The input graph is connected

## Prim's Algorithm
Named after Robert C. Prim who discovered the algorithm in 1957. However, it was
discovered that Vojtěch Jarník discovered it in 1930, so it is sometimes know as
the Prim-Jarník algorithm.

### Asymptotic Time Complexity
![O(m\log n))](https://latex.codecogs.com/gif.latex?O(m\log&space;n))

### Pseudo Code

#### Naive 
The pseudo code below is a particularly slow implementation
(![O(mn)](https://latex.codecogs.com/gif.latex?O(mn))) of Prim's algorithm.
However, this simple implementation makes the algorithm easy to understand.

```pseudeo
G = input graph
T = empty set (will contain edges in MST)
cost = 0

find_smallest:
    G = input graph
    smallest = NULL

    // Loop through each edge where tail vertex is conquered and head vertex is not conqured
    for each vertex (v) in G where v is conquered:
        for each edge in v where edge->head is NOT conquered:
            if smallest is NULL OR edge->weight < smallest->weight:
                smallest = edge

    return smallest

mark first vertex conquered

e = find_smallest G
while e != NULL:
    add e to T
    cost += e->weight
    mark e->head conquered
    edge = find_smallest G

return T and cost
```

#### Heap-Based Implementation
This version is more complex, but results in a considerable faster run time
(![O(m\log n))](https://latex.codecogs.com/gif.latex?O(m\log&space;n)))

Invariants:
- Heap contains vertices that have that have not yet been spanned
- Highest priority element in the heap is the next vertex to be conquered

``` pseudeo
G = input graph
T = empty set (will contain edges in MST)
H = empty heap
cost = 0

find_winning_edge:
    edges = edges to search

    return cheapest edge where head is conquered
    return NULL if no edges qualify

mark first vertex conquered

for v that is not conquered:
    winner = find_winning_edge v->edges
    if winner is NULL:
        score = infinity
    else
        score = winner->weight

    add v to H using score as the heap key value
        
while H is not empty:
    v = extracted min from H

    mark v as conquered
    cost += v.score
    add winner to T (calculated in previous loop)

    for every edge in v where the head is not conquered:
        winner = cheapest edge in edge->head->edges
        if winner is NULL:
            score = infinity
        else
            score = winner->weight

        recacluate H for winner->head
```

## Kruskal's Algorithm
Discovered in 1956 by ... Kruskal

### Asymptotic Time Complexity
![O(m\log n))](https://latex.codecogs.com/gif.latex?O(m\log&space;n))
