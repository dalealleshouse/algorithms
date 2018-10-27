# Graph Search
#graph

Prerequisite: [Graph Concepts](../graph_concepts)

Any aspiring computer scientist should make understanding graph search
algorithms a priority because of their ubiquity. Graph search is a common
problem in computer science because of it's wide range of applications. Luckily,
there are many blazingly fast solutions. There are two purposes for graph search
algorithms. The first, and obvious, purpose is to locate a particular node.  The
second is to identify vertices *reachable* from a specified vertex. A vertex is
reachable from another vertex if there is a path through the graph connecting
the two vertices.

There are two primary types of graph search algorithms: breadth first and depth
first. Both have the same goal - efficiently search a graph by only visiting
each node once.

As a matter of note, for most purposes graph search works essentially the same
in both directed and undirected graphs. The exception the rule is when checking
connectivity.

## Asymptotic Complexity
The asymptotic complexity of every algorithm presented in this section is
![O(m+n)](https://latex.codecogs.com/gif.latex?O(m&plus;n)). The actual run
times are only slightly higher than the amount of time required to read the
data. Because of this, graph search algorithms are considered *free primitives*.

## Breadth First Search (BFS)

BFS examines each edge of a particular vertex before following any edges of
connected vertices.


## BFS - Shortest Path

One unique attribute of BFS is that with only a few extra lines of code, it can
be used to calculate the shortest path between two nodes.

## Depth First Search (DFS)

DFS is analogous to searching a maze in that it dives as deep into the graph as
possible and only back tracks when absolutely necessary. One advantage is that
it can find the topological ordering of directed acyclic graphs.

## Applications
* Connectivity - In a network, ensure that every node is reachable from any
    node. Another example is determining degrees of separation such as finding a
    [Erdos number](https://en.wikipedia.org/wiki/Erd%C5%91s_number), or the more
    recent [Bacon number](https://oracleofbacon.org/).
* Shortest Path - Shortest distance between two places on a map (driving
    directions).
* Planning - Find the sequence of decisions from one point to another. For
    example, the sequence steps required to solve a puzzle.
* Connected Components - 
