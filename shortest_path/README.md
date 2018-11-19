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

## Dijkstra's
Dijkstra's shortest path algorithm calculates the shortest distance between
vertices in a *weighted graph with **non-negative** edge weights*. This is
without a doubt one of the *greatest hits* of algorithms with countless
applications. Those wishing to excel in computer science should take extra car
to ensure they fully understand this.

The algorithm as shown in the this section is a naive implementation. Each
iteration of the main while loop requires a scan over all edges to find the one
with the minimum distance. While this runtime isn't horrible, it's possible to
do much better using a [priority queue](../queue/README.md#priority-queue).

### Asymptotic Complexity
![O(mn)](https://latex.codecogs.com/gif.latex?O(mn))

### Pseudo Code
```
G = input graph
v = starting vertex
side effects: marks all vertices with the shortest distance from the starting
vertex

S = All vertices in G
for each vertex in S:
    vertex.distance = infinity

v.distance = 0

while S is not empty:
    u = vertex in S having the minimum distance
    remove u from Q

    for each outgoing edge in u:
        distance = u.distance + edge.weight
        if distance < edge.head.distance
            edge.head.distance = distance
```

## Dijkstra's w/ Priority Queue
By making a small change to the way in which the vertex with the minimum
distance is determined, it's possible to reduce the running time of Dijkstra's
algorithm. The trick is to us a priority queue with insert and delete operations
with an asymptotic complex of ![O(lg
n)](https://latex.codecogs.com/gif.latex?O(\log&space;n)) to keep track of the
current minimum distance.

### Asymptotic Complexity
![O(mlogn)](https://latex.codecogs.com/gif.latex?O(m\lg&space;n))

### Pseudo Code
```
G = input graph
v = starting vertex
Q = priority queue using vertex.distance as the key
side effects: marks all vertices with the shortest distance from the starting
vertex

v.distance = 0

for each vertex in S:
    if vertex is not source
        vertex.distance = infinity
    add vertex to Q


while Q is not empty:
    u = Q.extract_minimum

    for each outgoing edge in u:
        distance = u.distance + edge.weight
        if distance < edge.head.distance
            edge.head.distance = distance
            Q.reprioritize(edge.head)
```
