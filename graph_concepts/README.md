# Graph Concepts
#graph

This project contains many graph algorithms. This section provides a brief
overview of graph concepts. Consider comprehension of this material as a
pre-requisite for graph algorithms.

Graphs represent pair wise relationships among objects. In graph jargon, the
objects are known as either *vertices* or *nodes*. The relationship between the
vertices are *edges*. To make the concept a bit more concrete, consider modeling
one's family as a graph. The people in the family would be vertices, and the
relationships between them (mother, daughter, father, brother, etc..) would be
edges. Edges are typically denoted by a pair. If Dick is Jane's brother, the
relationship is be denoted as (Dick, Jane). The set of all vertices is denoted
as `V` and `E` denotes the set of all edges.

There are two types of graphs: *undirected* and *directed*. The edges of an
undirected graph indicate a bi-directional relationship. (A, B) and (B, A) are
essentially the same.  See the example below.

#### Undirected Graph
![graph](graph.png)

* `V`: A, B, C, D
* `E`: (A, B), (A, C), (A, D), (B, D), (C, D)

Conversely, the edges of a directed graph denote the direction of the
relationship. (A, C) indicates that A has a connection to C, but that does not
imply that the reverse is also true. Continuing on with the family graph
example, the brother edge (Dick, Jane) indicates that Dick is Jane's brother.
However, Jane is not Dick's brother. The end point of the relationship is known
as the *head* while the origin is referred to as the *tail*. Some sources also
refer to directed edges as *arcs*. See the example below.

#### Directed Graph

![directed graph](dir-graph.png)

* `V`: A, B, C, D
* `E`: (A, C), (A, D), (B, A), (B, D), (D, A), (D, C)

Edges can also have weights associated with them. Weighting assigns a cost to
each edge. Weighting can be modeled in almost any way imaginable. In a network,
this weight could be connection latency. Going back to the family graph example.
Each relationship edge could have an intimacy weight.  For instance, a spousal
relationship may have a weight of 5 whereas a sibling relationship may have a
weight of 3. See the image below. In this graph, the (Janice, Richard) edge has
a higher weight than the (Jane, Dick) edge.

#### Edge Weights

![weighted graph](weight-graph.png)

A term commonly used in graph applications is *parallel edge*. A parallel edge
is two or more edges related to the same two vertices. For instance, (A, B) and
(B, A) are parallel edges. The edges in the graphic below are parallel edges.

#### Parallel Edges

![parallel edges](par-edges.png)

## Graph Size

In order to accurately predict the performance of graph algorithms, it's
important to have common terminology for expressing the size of a graph. The
size is made up of two components: the number of vertices and the number of
edges. By convention, `n` denotes the number of vertices and `m` denotes the
number of edges.

Graph density is important when determining the best way to represent a graph in
memory. Consider a graph with no parallel edges, the lower bound of `m` is
![n-1](https://latex.codecogs.com/gif.latex?n-1). The upper bound of `m` is
![n(n-1)/2](https://latex.codecogs.com/gif.latex?n(n-1)/2). Roughly defined, a
*sparse* graph is closer to the lower bound and a *dense* graph is closer to the
upper bound.

## Graph Representations

There are several ways of representing a graph. Two of the most common are
adjacency matrices and adjacency lists.  The choice of which to use depends on
graph density (sparse vs dense) as well as the desired graph operations.

For both cases, consider the graph below.

![directed graph](dir-graph.png)

### Adjacency Matrix
An adjacency matrix depicts vertices as indices across the top and side of a
matrix. A one at the intersection of two vertices indicates a connected edge
while a zero indicates no connection. The associated adjacency matrix for the
graph in question is shown below.

![adjacency matrix](adj-matrix.png) 

The amount of space required to store an adjacency matrix is
![n^2](https://latex.codecogs.com/gif.latex?n^2).

### Adjacency List

There are several different ways to create an adjacency list. However, the
general concept is to have four components:

* Array of Vertices
* Array of Edges
* Edges have two pointers, one for the tail and one for the head
* Vertices have one pointer for each of its edges

One common adjacency list representation is an array of linked lists. Each item
in the array is a vertex. Each edge is represented as a link.

```
Vertices
[
    { A -> C -> D -> NULL }
    { B -> A -> D -> NULL }
    { C -> NULL }
    { D -> C -> A -> NULL }
]
```

Depending on the application's needs, keeping the vertices and edges in separate
arrays may be more appropriate as show below.

```
Vertices                Edges
[                       [
    { A, [0, 1] }           [0]{ A, C }
    { B, [2, 3] }           [1]{ A, D }
    { C, NULL }             [2]{ B, A }
    { D, [4, 5] }           [3]{ B, D }
]                           [4]{ D, C }
                            [5]{ D, A }
                        ]
```

Adjacency lists have lower space requirements than matrices. Additionally, they
are optimized for graph search algorithms.

## Common Uses 

There are virtually countless applications for graphs. Below are just a few
common uses.

* Road Networks (driving directions) - roads are vertices are intersections are
    edges
* Networks - machines are vertices and connections are edges
* Social Networks - people are vertices and relationships are edges
* Web - pages are the vertices and hyperlinks are the edges
* Genealogy - people are the vertices and family relationships are the edges
* Academic Plans: Courses are vertices and prerequisites are directed edges
