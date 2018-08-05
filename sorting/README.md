# Sorting

Typically, formal algorithm courses begin with sorting b/c they provide an easy
foundation for which to teach the fundamental concepts of asymptotic time
complexity. It seems fitting to group all these algorithms into a single section
in order to easily compare and contrast them. This section covers 5 different
algorithms:

1) [Bubble Sort](#bubble-sort)
1) [Insertion Sort](#insertion-sort)
1) [Selection Sort](#selection-sort)
1) [Merge Sort](#merge-sort)
1) [Quick Sort](#quick-sort)

## Actual Run Times

The actual run times for sorting various size integer arrays using the C
implementation of the algorithms is shown below. All data was collected using a
docker container running on a Surface Book 2 Laptop (Intel Core i7, 16GB RAM).
Each time value represents the median of 3 separate executions.

### Arrays w/ Random Integer Values

![alt text](c/RANDOM.png "RANDOM ARRAY VALUES") 

|ALGORITHM| n=100 |n=1000 |n=10000 |n=100000 |
--|--|--|--|--|
|C_QSORT |0.000005 sec|0.000060 sec|0.000799 sec|0.009573 sec|
|BUBBLE |0.000094 sec|0.003827 sec|0.453658 sec|41.579646 sec|
|INSERTION |0.000023 sec|0.001044 sec|0.080031 sec|8.820152 sec|
|SELECTION |0.000038 sec|0.000939 sec|0.082957 sec|9.374885 sec|
|QUICK |0.000010 sec|0.000124 sec|0.001693 sec|0.020595 sec|
|MERGE |0.000007 sec|0.000118 sec|0.001249 sec|0.015185 sec|

Key Takeaways: 
- *Quick*, *Merge*, and *C's qsort* all perform fairly consistently regardless of
    the order of the input array.
- *Bubble*, *Insertion*, and *Selection* perform well when the input size is
    low; however, the run time increases exponentially as the input size
    increases
- *Merge* performs slightly better than *Quick*, however, it has much higher
    memory requirements.
    
### Arrays w/ Pre-Sorted Values

![alt text](c/SORTED.png "SORTED ARRAY VALUES") 

|ALGORITHM| n=100 |n=1000 |n=10000 |n=100000 |
--|--|--|--|--|
|C_QSORT |0.000001 sec|0.000016 sec|0.000178 sec|0.002445 sec|
|BUBBLE |0.000000 sec|0.000002 sec|0.000016 sec|0.000158 sec|
|INSERTION |0.000002 sec|0.000013 sec|0.000129 sec|0.001444 sec|
|SELECTION |0.000009 sec|0.000848 sec|0.082737 sec|9.032020 sec|
|QUICK |0.000027 sec|0.000093 sec|0.001232 sec|0.015116 sec|
|MERGE |0.000003 sec|0.000068 sec|0.000369 sec|0.004767 sec|

Key Takeaways of pre-sorted arrays: 
- *Bubble* clearly performs best
- *Insertion* outperforms *Selection* by a wide margin (opposite of reverse
    sorted results)

### Arrays w/ Reverse Sorted Values

![alt text](c/REVERSED.png "REVERSED ARRAY VALUES") 

|ALGORITHM| n=100 |n=1000 |n=10000 |n=100000 |
--|--|--|--|--|
|C_QSORT |0.000002 sec|0.000019 sec|0.000223 sec|0.002760 sec|
|BUBBLE |0.000044 sec|0.004318 sec|0.429120 sec|48.229245 sec|
|INSERTION |0.000031 sec|0.002819 sec|0.169636 sec|17.977106 sec|
|SELECTION |0.000013 sec|0.000995 sec|0.094138 sec|10.214144 sec|
|QUICK |0.000008 sec|0.000107 sec|0.001442 sec|0.017348 sec|
|MERGE |0.000004 sec|0.000040 sec|0.000475 sec|0.005245 sec|

Key Takeaways of reverse arrays: 
- *Bubble* is clearly the worst performer
- *Selection* outperforms *Insertion* by a wide margin (opposite of
    pre-sorted results)

## Bubble Sort

## Insertion Sort

## Selection Sort

## Merge Sort

Merge sort is a classic divide and conquer algorithm. It splits an array in
half, recursively sorts each half, and finally merges the halves together.

### Asymptotic Time Complexity
O(n log(2, n))

### Pseudo Code

``` pseudo
sort:
    A = input array
    returns: New array consisting of sorted values

    if length of A == 1:
        return A

    A1 = first half of A
    A2 = second half of A

    sort(A1)
    sort(A2)

    return merge(A1, A2)

merge:
    A1 = sorted array
    A2 = sorted array
    returns: new array with values merged from A1 and A1

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




## Quick Sort

Sorting algorithm that is prevalent in practice. Although it has the same
runtime on average as merge sort, it will execute faster due to reduced memory
requirements. Additionally, it is far easier to implement than quick sort.

One important consideration is the implementation of Choose Pivot. Choosing a
pivot element that represents the median value in the array is ideal. However,
the overhead associated with calculating the median renders the approach
ineffective. There are mathematical proofs that demonstrate choosing a pivot
point at random will ensure a O(n log(2, n)) on average.

### Asymptotic Time Complexity
O(n log(2, n)) _on average_

### Pseudo Code

``` pseudo
sort:
    // side effect: rearranges the values in A
    A = input array
    n = length of A

    if n <= 1:
        return

    pivot = choose_pivot(n)
    swap A[0] and A[pivot]

    pivot_index = partition(A[0])

    // The array is never copied, all swaps happen in place.
    // Recursive calls pass a reference to a portion of the array.
    sort(A[0 thru pivot_index - 1])
    sort(A[pivot_index + 1 thru n])

partition:
    // side effects:
    // - All items in A that are less than the value at A[0] are before A[return value]
    // - All items in A that are greater than the value at A[0] are moved after A[return value]
    // - The value at A[0] is moved to A[return value]
    returns: new position of the value at A[0] after the partition
    A = input array
    n = number of elements to partition around

    index = 1
    for i = 1 to n:
        if A[i] < A[0]:
            swap A[i] and A[index]
            increment index

    if index - 1 > 0:
        swap A[0] and A[index]

    return index

choose_pivot:
    returns: ideal index to partition on
    n = number of elements in the partition

    return uniformly random number between 0 and n inclusive
```
