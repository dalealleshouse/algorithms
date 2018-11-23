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
between nodes. This is only valid for *non-weighted* graphs.

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

## Dijkstra's (Naive)
Dijkstra's shortest path algorithm calculates the shortest distance between
vertices in a *weighted graph with **non-negative** edge weights*. This is
without a doubt one of the *greatest hits* of algorithms with countless
applications. Those wishing to excel in computer science should take extra car
to ensure they fully understand this.

The algorithm as shown in the this section is a naive implementation. Each
iteration of the main while loop requires a scan over all edges to find the one
with the minimum distance. While this runtime isn't horrible, it's possible to
do much better using the implementation in the next section.

### Asymptotic Complexity
![O(mn)](https://latex.codecogs.com/gif.latex?O(mn))

### Pseudo Code
```
dijkstra:
    G = input graph with distance of each vertex set to infinity
    v = starting vertex
    side effects: marks all vertices with the shortest distance from the starting
    vertex

    S = empty array

    v.distance = 0
    S.add(v)

    while true:
        u = find_min(G, S)
        if u is NULL or u.distance == infinity:
            break

        S.add(u)

find_min:
    G = input graph
    S = array containing all conquered vertices

    v = NULL

    for each vertex in S:
        for each edge in vertex:
            if edge.head is contained in S:
                continue

            distance = edge.tail.distance + edge.weight
            if distance < edge.head.distance
                edge.head.distance = distance

            if edge.head.distance < v.distance
                v = edge.head

    return v
```

## Dijkstra's
By making a small change to the way in which the vertex with the minimum
distance is determined, it's possible to reduce the running time of Dijkstra's
algorithm. The trick is to us a [heap](../data_structures/README.md#heap).

### Asymptotic Complexity
![O((m + n)log n)](https://latex.codecogs.com/gif.latex?O((m + n)\lg&space;n))

### Pseudo Code
```
G = input graph with distance of each vertex set to infinity
v = starting vertex
H = head using vertex.distance as the key
side effects: marks all vertices with the shortest distance from the starting
vertex

v.distance = 0
H.insert(v)

while H is not empty:
    u = H.extract_minimum

    for each outgoing edge in u:
        if edge.head has already been processed, skip it

        distance = u.distance + edge.weight
        if distance < edge.head.distance
            edge.head.distance = distance

            if edge.head exists in H:
                // Force reorder of keys
                H.delete(edge.head)
                H.insert(edge.head)
            else
                H.insert(v)
```
