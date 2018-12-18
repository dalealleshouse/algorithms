# Running Median

This algorithm showcases how useful [Data Structures](../data_structures) are
for solving real-world programming problems.

Consider being tasked with performance critical computation of real time data
statistics. It's fairly easy to imagine calculating the max and min values for a
dynamic list of numbers using a [Binary
Tree](../list_data_structures/README.md#binary-trees). Binary Trees provide
Insert, Delete, Min, and Max operations with an asymptotic complexity of
![O(log(2,n)](https://latex.codecogs.com/gif.latex?O(\log_{2}n)). Another option
is maintaining two [Heap](../data_structures/README.md#heap)s: one for the max
value and one for the min value. Not a bad choice either because it's possible
to calculate min and max in constant time with the trade off of maintaining two
data structures instead of one. Regardless of the chosen implementation, it's
possible to find min and max in logarithmic time. Is it feasible to do the same
with median?

The only data structure presented as of yet that supports calculating median
values is [Sorted Array](../list_data_structures/README.md#sorted-arrays)s. In
fact, median values are calculable in constant time with sorted arrays as is
shown in the pseudo code below.

### Sorted Array Median Pseudo Code
```
meidan:
    SA = zero-based sorted array
    n = number of items in array

    middle_index = floor of n / 2

    if n is odd:
        return SA[median_index]
    else // n is even
        return (SA[median_index] + SA[median_index - 1]) / 2
        
```

The obvious problem is that sorted arrays are notoriously inadequate for
handling insert and delete operations. For this use case, the data is constantly
changing. Sorted Arrays just aren't an option. So, if Sorted Arrays are the only
data structure presented as of yet that supports calculating medians and they
aren't viable, it's time to get creative with what's available.

Recall that a Heap can calculate either a min or max, but not both, in constant
time. Storing the lower half of the numbers in a max heap and the upper half in
a min heap allows for insert and delete operations in logarithmic time as well
as median calculations in constant time. This is a bit difficult to envision
without examining the pseudo code.

### Running Median Pseudo Code
```
global variables:
    upper_heap = Heap holding upper half of the numbers, tracks min
    lower_heap = Heap holding lower half of the numbers, tracks max
    n = total items being tracked

insert:
    x = new_value

    if x is greater than lower_heap->find:
        upper_heap->insert(x)
    else
        lower_heap->insert(x)

    increment n

    if is_balanced is false:
        if upper_heap->n > lower_heap->n:
            lower_heap->insert(upper_heap->extract())
        else
            upper_heap->insert(lower_heap->extract())

median:
    if lower_heap->n = upper_heap->n:
        return (lower_heap->find + upper_heap->find) / 2

    return lower_heap->find

is_balanced:
    if n is even:
        return lower_heap->n == upper_heap->n

    return lower_heap->n == (upper_heap->n + 1)
```

## Asymptotic Analysis

* Median: ![O(1)](https://latex.codecogs.com/gif.latex?O(1))
* Insert: ![O(log(2,n)](https://latex.codecogs.com/gif.latex?O(\log_{2}n))
