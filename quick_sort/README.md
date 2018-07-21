# Quick Sort

Sorting algorithm that is prevalent in practice. Although it has the same runtime on average as merge sort, in practice
it will execute faster due to reduced memory requirements. Additionally, it is far easier to implement than quick sort.

One important consideration is the implementation of Choose Pivot. Choosing a pivot element that represents the median
value in the array is ideal. However, the overhead associated with calculating the median renders the approach
ineffective. There are mathematical proofs that demonstrate choosing a pivot point at random will ensure a O(n log(2,
n)) on average.

## Runtime
O(n log(2, n)) _on average_

## Pseudo Code

### QuickSort
Inputs:
- A = input array
- n = length of A

Side Effects:
- Sorts the values in A

``` pseudo
if n <= 1:
    return

pivot = ChoosePivot(n)
swap A[0] and A[pivot]

pivot_index = Partition(A[0])

// The array is never copied, all swaps happen in place.
// Recursive calls pass a reference to a portion of the array.
QuickSort(A[0 thru pivot_index - 1])
QuickSort(A[pivot_index + 1 thru n])
```

### Partition
Inputs:
- A = input array
- n = number of elements to partition around

Assumptions:
- A[0] is the value to partition around

Returns:
- New position of the value at A[0] after the partition

Side Effects:
- All items in A that are less than the value at A[0] are before A[return value]
- All items in A that are greater than the value at A[0] are moved after A[return value]
- The value at A[0] is moved to A[return value]

``` pseudo
index = 1
for i = 1 to n:
    if A[i] < A[0]:
        swap A[i] and A[index]
        increment index

if index - 1 > 0:
    swap A[0] and A[index]

return index
```

### Choose Pivot
Inputs:
- n = number of elements in the partition

Returns:
- ideal index to partition on


``` pseudo
return uniformly random number between 0 and n inclusive
```
