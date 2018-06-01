# Merge Sort

Merge sort is a classic divide and conquer algorithm for sorting an array.

## Runtime
O(n log(n))

## Input
- Unsorted array of items
- Comparator function that accepts two arguments x and y. Return -1 if x should
    be sorted before y, 0 if the items are equal, and 1 if x should sorted
    after y

## Output
Sorted array

## Pseudo Code

```
A = input array

C = recursively sort first half of A
D = recursively sort second half of A

return merge(C, D)

merge:
    i = 1
    j = 1

    for k = 1 to n
        if C[i] < D[j]
            B[k] = C[i]
            i++
        else
            B[k] = D[j]
            j++
            
```



