# Shortest Path Algorithms
#graph, #graph-search

Prerequisite: [Graph Concepts](../graph_concepts), [Graph
Search](../graph_search)

 *Shortest Path* is defined as the path through a graph for which the sum of the
 edge weights is the smallest. In unweighted graphs, this is sometimes referred
 to as *degrees of separation*. Calculating the shortest path is a commonly
 encountered challenge for professional programmers. Luckily, there are many
 existing well defined algorithms that fit almost any situation. 

## BFS
One unique attribute of
[BFS](../graph_search/README.md#breadth-first-search-bfs) is that with only a
few extra lines of code, it can be used to calculate the degrees of separation
between nodes. This is only valid for *non-weighted undirected* graphs.

### Asymptotic Complexity
![O(m+n)](https://latex.codecogs.com/gif.latex?O(m&plus;n))

### Pseudo Code
<pre>
G = input graph
q = queue data structure
v = starting vertex
side effects: marks all vertices with the degrees of seperation from v

conquer(v)
<b>v->distance = 0</b>
q->enqueue(v)

while q is not empty:
    v = q->dequeue
    for each edge in v:
        w = edge->head
        if w is NOT conquered:
            conquer(w)
            <b>w->distance = v->distance + 1</b>
            q->enqueue(w)
</pre>

## Dijkstra's
Dijkstra's shortest-path algorithm calculates the shortest distance between
vertices in a *weighted graph with **non-negative** edge weights*. *Shortest
Path* is defined as the path for which the sum of the edge weights is the
smallest.
