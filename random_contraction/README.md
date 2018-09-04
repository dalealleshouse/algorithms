# Random Contraction
#graph, #randomized

Prerequisite: [Graph Concepts](../graph_concepts)

The random contraction algorithm finds the [minimum
cut](../graph_concepts/README.md#minimum-cut) of a graph. It accomplishes this
by randomly contracting edges until only two vertices remain (see pseudo code).
Because a minimum cut has fewer edges than a non-minimum cut, the algorithm has
a higher probability of producing a minimum cut than a non-minimum cut. The key
concept here is that it has a *higher probability* of producing a minimum cut:
it is not guaranteed to produce one.

The probability that random contraction produces a minimum cut is
![2/n(n-1)](https://latex.codecogs.com/gif.latex?\frac{2}{n(n-1)}). In order to
ensure that a minimum cut is produced, the algorithm should be run ![n^2 log
n](https://latex.codecogs.com/gif.latex?n^2&space;\log&space;n) times. The
smallest minimum cut found over all executions  has a high probability of being
an actual minimum cut.

## Asymptotic Time Complexity

![O(n^2m](https://latex.codecogs.com/gif.latex?O(n^2m)) per single execution.
![O(n^4 log n](https://latex.codecogs.com/gif.latex?O(n^4&space;\log&space;n))
total for finding a minimum cut with a high probability of being correct.




-----------------------------------
Everything below this line is a work in progress

I don't understand this...
Watch counting minimum cuts again
what is the largest number of min cuts that a graph with n vertices can have The
lower bound is ![n choose 2](https://latex.codecogs.com/gif.latex?\binom{n}{2})
or ![n(n-1)/2](https://latex.codecogs.com/gif.latex?\frac{n(n-1)}{2}) minimum
cuts in a graph.


Probability of success is ![1/n^2](https://latex.codecogs.com/gif.latex?1/n^2)

Must run multiple times to get a resonable proablity of success

N = number of trails needed

n^2 trails produces ~1/3 probablity of failure

n^2 lnn = 1/n failures proablabity



## Asymptotic Time Complexity

![O(n^2m](https://latex.codecogs.com/gif.latex?O(n^2m))

## Pseudo Code

Inputs: [Adjacency List](../graph_concepts/README.md#adjacency-list)
representing an undirected graph (parallel edges allowed)

Output: Minimum Cut



while n > 2:
pick edge at random
merge end into single vertex
remove self loops

return cut represented by final 2 vertices

I'm code

## Common Uses

* Reliability Analysis: Consider a network graph. The minimum cut represents the
    minimum number of failures that will cause a outage.
* Social Networking: Identify sub-cultures by finding portions of the graph that
    are highly interconnected yet weakly connected to the graph as a whole
* Image Segmentation: Pixels are vertices and edges are defined by adjacency.
    Edge weight is the difference between pixel colors. Minimum cuts should be
    able to isolate individual objects in the picture.
