# Inversion Counting

Count the number of inversions in an array. An inversion is two items that are
out of order. For instance, in the array:

[1][3][2]

the numbers 3 and 2 represent an inversion.

The typical use of such algorithms is comparing preferences between users. The
number of inversions between user A's ranking of products and users B's ranking
of products represents how similar their affinities are.

This algorithm is essentially the same as merge sort with the exception that it
keeps track of inversions while sorting.

## Runtime
O(n log(n))

## Input
- Array of items

## Output
Number of inversions in the array

## Pseudo Code

```
A = input array
inversions = 0

if length of A is 1
    return 0

count:
    A1 = first half of A
    A2 = second half of A

    inversions += recursively count\sort A1
    inversions += recursively count\sort A2

    i = 1
    j = 1

    for k = 1 to n
        if A1[i] < A2[j]
            A[k] = A1[i]
            i++
        else
            A[k] = A2[j]
            j++
            inversions += # of items remaining in A1

    return inversions
            
```



