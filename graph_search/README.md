# Graph Search
#graph

Prerequisite: [Graph Concepts](../graph_concepts)

Any aspiring computer scientist should make understanding graph search
algorithms a priority because of their ubiquity and wide range of applications.
There are two purposes for graph search. The first, and obvious, purpose is to
locate a particular node. The second is to identify vertices *reachable* from a
specified vertex. A vertex is reachable from another vertex if there is a path
through the graph connecting the two vertices. While this may seem parochial,
determining reachability is the cornerstone of more advanced analysis.

There are two primary types of graph search algorithms: breadth first and depth
first. Both have the same goal - efficiently search a graph by only visiting
each node once. As demonstrated below, they have the same basic structure with
only slight deviations which make them uniquely suited for different purposes.

Unless otherwise specified, the graph search algorithms presented here work
essentially the same for both directed and undirected graphs.

## Asymptotic Complexity
The asymptotic complexity of every algorithm presented in this section is
![O(m+n)](https://latex.codecogs.com/gif.latex?O(m&plus;n)). The actual run
times are only slightly higher than the amount of time required to read the
data. Because of this, graph search algorithms are considered *free primitives*.

## Breadth First Search (BFS)

BFS examines each edge of a particular vertex before following any edges of
connected vertices. The key to this is the use of a [Queue](../queue) as shown
in the pseudo below.

### Pseudo Code

```
G = input graph
q = queue data structure
v = starting vertex
side effects: marks all vertices conquered that are reachable from v

conquer(v)
q->enqueue(v)

while q is not empty:
    v = q->dequeue
    for each edge in v:
        w = edge->head
        if w is NOT conquered:
            conquer(w)
            q->enqueue(w)
```

## BFS - Shortest Path

One unique attribute of BFS is that with only a few extra lines of code, it can
be used to calculate the shortest path between two nodes. The code below marks
each vertex with the degrees of separation from the input vertex.

### Pseudo Code

<pre>
G = input graph
q = queue data structure
v = starting vertex
side effects: marks all vertices conquered that are reachable from v

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

## Connected Components (Undirected Graphs Only)

This algorithm identifies the [connected
components](../graph_concepts/README.md#connected-components) in a graph by
marking each vertex with a component id. Any vertex with a matching id is part
of the same connected component. Differing ids indicate no connectivity.
Although shown below with BFS, DFS works equally well for this purpose.

```
G = input graph
q = queue data structure
side effects: marks all vertices with a component id

component_id = 0

for each vertex (v) in G:
    if v is NOT conquered:
        component_id = component_id + 1
        conquer(v)
        v->component_id = component_id
        q->enqueue(v)

        while q is not empty:
            v = q->dequeue
            for each edge in v:
                w = edge->head
                if w is NOT conquered:
                    conquer(w)
                    w->component_id = component_id
                    q->enqueue(w)
 ```

## Depth First Search (DFS)

DFS is analogous to searching a maze in that it dives as deep into the graph as
possible and only back tracks when absolutely necessary. It is identical to BFS
with two exceptions. First, it uses a [Stack](../stack) instead of a Queue.
Second, items are concured after being `pop`ed from the stack instead of before
they are `enqueue`ed.

## Applications
* Connectivity - In a network, ensure that every node is reachable. 
* Shortest Path - Shortest distance between two places on a map (driving
    directions). Another example is determining degrees of separation such as
    finding a [Erdos number](https://en.wikipedia.org/wiki/Erd%C5%91s_number),
    or the more recent [Bacon number](https://oracleofbacon.org/).
* Planning - Find the sequence of decisions from one point to another. For
    example, the sequence steps required to solve a puzzle.
