# Greedy Algorithms
    - Iteratively make myopic irrevocable decisions
    - Dijkstra's shortest path

### Pros
- Easy to apply
- Easy run time analysis

### Cons
- Difficult to establish correctness
- Despite intuition, most are NOT correct
    - Heuristics are often misleading

Correctness Proofs
- Induction
- Exchange argument
- Whatever works

The caching problem
- Von Neumann architecture memory hierarchy
- The optimal caching algorithm is a greedy algorithm - furthest in the future
    algorithm
    * this is unimplementable but serves as a guideline for practical algorithms 
    * informs that Least Recently Used (LRU) algorithm - gold standard for cache
        algorithms
    * serves as an idealized benchmark

# Dynamic Programming
- Similar to divide and conquer problem, except sub-problems may "overlap". The
    solution to sub-problems is stored in a table.
- Optimization Problem - there are many potential solutions. Each solution has
    an associated value and the goal is to find a optimal (maximum or minimum)
    value. There are many optimal solutions - the goal is find any one of them.

Steps in dynamic programming algorithm
1. Define the structure of an optimal solution
1. Recursively define the value of an optimal solution
1. Compute the value of an optimal solution
1. Construct optimal solution from the computed information
