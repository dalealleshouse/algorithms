# List Data Structures

A *list* (aka *collection* or *container*) is a logical grouping of data items
into a single unit. Lists enable operations such as inserting new items,
searching through existing items, deleting items, and enumerating existing
items. There are several common list data structures and different structures
have different capabilities/performance characteristics.

In the author's humble opinion, comprehension of list data structures is the
first concept that an aspiring programmer should master because list
manipulation is the single most common programming task. So common in fact that
all mainstream languages have builtin list abstractions.  For instance, LISP,
which stands for LISt Processor, is designed entirely around the concept.  C#
has the `IEnumerable` interface, Java has the `Iterable` interface, Python has
`list`. There are many more examples but there is no need to belabor the point.

List data structures have differing strengths, weaknesses, and capabilities.
Therefore, it's important to fully understand list abstractions in order to
choose the correct one. Sometimes choosing the right one is easy because a
particular utility is required. Contrarily, it's possible to choose one that
will meet the software requirements but have a prodigious negative impact on
performance.

This section examines four list data structures [Arrays](#arrays), [Linked
Lists](#linked-lists), [Binary Trees](#binary-trees), and [Hash
Tables](#hash-tables).

## Arrays
#data_structure, #list

An array is the most common and simple list data structure. It imposes no memory
overhead because the constituent items occupy a single contiguous section of
memory. The graphic below depicts how an array is arranged in memory.

#### Array
![Array](array.png)

### Asymptotic Complexity
- Insert\Delete: ![O(n)](https://latex.codecogs.com/gif.latex?O(n) "O(n)")
- Search: ![O(n)](https://latex.codecogs.com/gif.latex?O(n) "O(n)")
   - The [Binary Search](../binary_search) algorithm can perform array searches
       in ![O(log n)](https://latex.codecogs.com/gif.latex?O(\log&space;n)) if
       the array is sorted
- Enumerate: ![O(n)](https://latex.codecogs.com/gif.latex?O(n) "O(n)")

### Advantages
- *Access By Index*: Arrays are the only data structure capable of accessing
    items by index in ![O(1)](https://latex.codecogs.com/gif.latex?O(1) "O(1)").
    The memory address of any item is easily calculable by summing the base
    address and the product of the desired index and item size.
- *Memory*: Zero overhead required. The total size of an array is the sum of all
    its items.
- *Cache Optimized*: Guaranteed to have optimal [spatial
    locality](https://en.wikipedia.org/wiki/Locality_of_reference) which can
    have profound performance implications. See the [Memory
    Cache](../memory_cache/) section for more details.

### Disadvantages
- *Insertions*: Inserting a new item requires a larger contiguous section of
    memory. Therefore, a new section of memory must be allocated and existing
    items must be copied into the newly allocated area.
- *Deletions*: Deleting is subject to the same problems as insertions. Items
    must be stored contiguously; therefore, they have to be shifted either to
    the left or right to fill the missing void.

## Linked Lists
#data_structure, #list

Linked list is a simple yet powerful data structure. Much like an array, a
linked list is simply a collection of items. The difference is that linked list
items are not stored contiguously in memory. Each item maintains a pointer to
the next item in the list so the items can be located anywhere. This is depicted
graphically below.

#### Linked List
![Linked List](singly_linked_list.png)

By convention, the first item is the list is known as the *head* and the last
item is known as the *tail*.  There are essentially two types of linked lists:
*singly linked lists* (depicted above) and *doubly linked lists* (depicted
below). As the names imply, singly linked lists maintain a single pointer to the
next item. Doubly linked lists maintain a pointer to the next item as well as a
pointer the previous item.

#### Doubly Linked List
![Doubly Linked List](doubly_linked_list.png)

Linked lists are especially helpful for applications such as
[Stack](../stack/)s, and [Queue](../queue/)s.

### Asymptotic Complexity
- Insert\Delete: ![O(1)](https://latex.codecogs.com/gif.latex?O(1) "O(1)")
- Search: ![O(n)](https://latex.codecogs.com/gif.latex?O(n) "O(n)")
- Enumerate: ![O(n)](https://latex.codecogs.com/gif.latex?O(n) "O(n)")

### Advantages
- *Insertions*: Insertions to the list is a
    ![O(1)](https://latex.codecogs.com/gif.latex?O(1)) operation
- *Deletions*: Deleting from the list is a
    ![O(1)](https://latex.codecogs.com/gif.latex?O(1)) operation

### Disadvantages
- *Random access*: The only way to access items is sequentially by starting at
    the head and navigating through the `next` pointers.
- *Memory*: Each item in the list must maintain an additional pointer. The total
    size of a linked list is the sum of the items plus the size of the pointers
    times the number of items.
- *Cache Optimized*: It is possible to create a linked list with poor [spatial
    locality](https://en.wikipedia.org/wiki/Locality_of_reference) which can
    have profound performance implications. See the [Memory
    Cache](../memory_cache/) section for more details.

## Binary Trees
#data_structure, #list, #graph

### Asymptotic Complexity
- Insert\Delete: ![O(log
    n)](https://latex.codecogs.com/gif.latex?O(\log&space;n))
- Search: ![O(log n)](https://latex.codecogs.com/gif.latex?O(\log&space;n))
- Enumerate: N/A

## Hash Tables
#data_structure, #list









## BELOW IS A WORK IN PROGRESS




## Actual Run Times

The actual run times for performing operations on arrays and linked list are
shown below.  For details about how the calculations were run, see
[compare_times.py](c/compare_times.py) and [algo_timer.c](c/algo_timer.c). To
recreate the data on your machine, navigate to the c directory and execute the
[time_charts.sh](c/time_charts.sh) bash file.

### Insertions

All insertions were made to the head of the data structure. Each array insertion
results in a reallocation and copy.

![INSERTION RUN TIMES](c/INSERTION.png "INSERTION RUN TIMES") 

|STRUCTURE|n=100 |n=1000 |n=10000 |n=100000 |
|--|--|--|--|--|
|LINKED_LIST |0.000003 sec|0.000032 sec|0.000315 sec|0.002879 sec|
|ARRAY |0.000003 sec|0.000080 sec|0.005127 sec|0.584865 sec|

Key Takeaways:
- Inserting 100,000 items into a array is approximately a half second slower
    than inserting the same items into a linked list.
- The extra time is due to the array needed to be resized on each insertion.

### Random Access

The times below are for accessing an item at a random index inside a loop. The
code below show the actual code that is being timed.

```
// Array Random Access Code
for (size_t i = 0; i < n; i++)
    val += arr[rand() % (n - 1)];

// Linked List Random Access Code
for (size_t i = 0; i < n; i++) {
    void* item = list_get_at(&list, rand() % (n - 1));
    val += (uintptr_t)item;
}
```

![RANDOM ACCESS RUN TIMES](c/RANDOM_ACCESS.png "RANDOM ACCESS RUN TIMES") 

|STRUCTURE|n=100 |n=1000 |n=10000 |n=100000 |
|--|--|--|--|--|
|LINKED_LIST |0.000009 sec|0.000995 sec|0.084897 sec|4.125063 sec|
|ARRAY |0.000002 sec|0.000015 sec|0.000145 sec|0.001475 sec|

Key Takeaways:
- 100,000 random access operations are a whopping 4 seconds slower with a linked
    list than with an array.

### Sequential Access

Sequential access refers to reading all items in a data structure by starting at
the head and reading each item in order until the tail is reached. The actual
code is shown below.

```
// Array Sequential Access Code
for (size_t i = 0; i < n; i++)
    val += arr[i];


// Linked List Sequential Access Code
list_item* item = list.head;
while (item != NULL) {
    val += (uintptr_t)item->payload;
    item = item->next;
}
```

The *DISPERSED LINKED LIST* data point shows a linked list in which the items
are distributed across a large area of memory. Conversely, the *LINKED_LIST*'s
items are stored adjacent to each other in memory.

As a side note, there is a fair amount of minutia involved with allocating
memory using `malloc`. The individual items in the linked list will not be 100%
distributed as advertised.  However, this distinction holds no real bearing on
the validity of the results.


![SEQUENTIAL ACCESS RUN TIMES](c/SEQUENTIAL_ACCESS.png "SEQUENTIAL ACCESS RUN
TIMES") 

|STRUCTURE|n=100 |n=1000 |n=10000 |n=100000 |
|--|--|--|--|--|
|LINKED_LIST |0.000002 sec|0.000007 sec|0.000089 sec|0.001177 sec|
|ARRAY |0.000001 sec|0.000001 sec|0.000005 sec|0.000043 sec|
|DISPERSED_LINKED_LIST |0.000002 sec|0.000011 sec|0.000108 sec|0.003723 sec|

Key Takeaways:
- The time differences over 100,000 operations is around .003 seconds.
- The results demonstrate the importance [spatial
    locality](https://en.wikipedia.org/wiki/Locality_of_reference). Poor
    locality, as demonstrated by the dispersed linked list, can have performance
    penalties due to cache misses.
