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

sort:
    A1 = recursively sort first half of A
    A2 = recursively sort second half of A

    return merge(A1, A2)

merge:
    i = 1
    j = 1

    for k = 1 to n
        if A1[i] < A2[j]
            B[k] = A1[i]
            i++
        else
            B[k] = A2[j]
            j++

    return B
            
```



