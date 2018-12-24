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
#graph-search, #breadth-first-search

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

## Connected Components (Undirected)
#graph-search, #breadth-first-search

This algorithm identifies the [connected
components](../graph_concepts/README.md#connected-components-undirected) in a
graph by marking each vertex with a component id. Any vertex with a matching id
is part of the same connected component. Differing ids indicate no connectivity.
Although shown below with BFS, DFS works equally well for this purpose.

### Pseudo Code

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
#graph-search, #depth-first-search

DFS is analogous to searching a maze in that it dives as deep into the graph as
possible and only back tracks when absolutely necessary. It is identical to BFS
with two exceptions. First, it uses a [Stack](../stack) instead of a Queue.
Second, items are conquered after being `pop`ed from the stack instead of before
they are `enqueue`ed.

### Pseudo Code

```
G = input graph
s = stack data structure
v = starting vertex
side effects: marks all vertices conquered that are reachable from v

s->push(v)

while s is not empty:
    v = s->pop
    if v is NOT conquered:
        conquer(v)
        for each edge in v:
            w = edge->head
            s->push(w)
 ```

DFS also lends itself to a rather elegant recursive implementation that does not
require any additional data structures.

```
G = input graph
v = starting vertex
side effects: marks all vertices conquered that are reachable from v

conquer(v)

for each edge in v:
    w = edge->head
    if w is NOT conquered:
        recurse (G, w)
```

## Topological Ordering (Acyclic Directed Graphs)
#graph-search, #depth-first-search

Just as some applications, such as [shortest path](#bfs---shortest-path), only
work with BFS, [topological
ordering](../graph_concepts/README.md#topological-ordering) only works with DFS.
The algorithm laid out below discovers a valid topological ordering for the
input graph. One important thing to note is that this is only one of possibly
many.

```
Global variable: order

topo-sort:
    G = input graph

    order = |V| - number of vertices in G
    for every vertex in G:
        if vertex is NOT conquered:
            dfs-topo(G, vertex)

dfs-topo:
    G = input graph
    v = starting vertex
    side effects: marks every vertex with a topological sort order

    conquer(v)

    for each edge in V:
        w = edge->head
        if w is NOT conquered:
            dfs-topo(G, w)

    v->top-order = order
    order = order - 1
```

### Strongly Connected Components (Directed)
#graph-search, #depth-first-search

Finding the [strongly connected
components](../graph_concepts/README.md#strongly-connected-components-directed)
of a directed graph is the most complex of the algorithms shown here. This
algorithm (often referred to as the *Kosaraju* or *Kosaraju-Sharir* algorithm)
performs DFS twice. The first time it derives an ordering in which to process
the vertices that guarantees the second pass of DFS will start with a sink
component. In other words, the goal is to identify the reverse topological
ordering of all strongly connected components.  The second discovers the
strongly connected components.


```
strong-connected-components:
    G = input graph
    S = stack to hold ordering

    magic-ordering(G, S)
    set all vertices to NOT conquered

    while S is not empty:
        v = S->pop
        if v is NOT conquered:
            scc-dfs(G, v)
            
scc-dfs:
    G = input graph
    v = starting vertex
    scc_id = strongly connected component id to mark all vertices with
    side effects: marks all reachable vertices with a strongly connected
    component id

    s = new stack data structure
    s->push(v)

    while s is not empty:
        v = s->pop
        if v is NOT conquered:
            conquer(v)
            v->scc_id = scc_id
            for each edge in v:
                w = edge->head
                s->push(w)

// equivalent to a topological sort with the direction of the edges reversed
magic-ordering:
    G = input graph
    S = stack to hold ordering

    for every vertex in G:
        if vertex is NOT conquered:
            reverse-topo-sort(G, S, vertex)

reverse-topo-sort:
    G = input graph
    S = stack to hold ordering
    v = starting vertex
    side effect: places vertices on the stack in the "magic order"

    conquer(v)

    for each incoming edge in V:
        w = edge->tail
        if w is NOT conquered:
            reverse-topo-sort(G, S, w)

    S->push(v)
```

## Applications
* Connectivity - In a network, ensure that every node is reachable. 
* Shortest Path - Shortest distance between two places on a map (driving
    directions). Another example is determining degrees of separation such as
    finding a [Erdos number](https://en.wikipedia.org/wiki/Erd%C5%91s_number),
    or the more recent [Bacon number](https://oracleofbacon.org/).
* Topological Ordering - Determine the ideal order of tasks that have precedence
    constraints (e.g. university courses with pre-requisites or steps to solve a
    puzzle)
* Strongly Connected Components: Collapse a complicated graph into a more
    manageable graph.
