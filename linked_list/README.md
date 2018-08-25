# Linked Lists
#data_structure

Linked Lists are a simple yet powerful data structure with utility similar to an
array. Most high level languages provide a built in linked list. For instance,
C# and Java both have the aptly named `LinkedList<T>` construct.  A linked list
can provide utility that an array cannot. This utility is not free though. As
with most programming constructs, there are trade offs that must be carefully
considered.

In order to appreciate linked lists, one must understand how they are different
than arrays. Therefore, a bit of review is in order.

## Array

An array is stored in a single contiguous section of memory. The addresses of
any item is easily calculated by adding the base address to the product of the
desired index and the item size.  For example, see the array depicted below.

```
Address
 0x001         each item is 4 bytes
   |
{----}{----}{----}{----}{----}{----}{----}{----}{----}{----}{----}{----}
                    |
                 Index 3

Index 3 address = 0x001 + (3 * 4) = 0x010 (16 in decimal)
```

### Advantages
- *Random access*: As depicted above, access to any item is an
    ![O(1)](https://latex.codecogs.com/gif.latex?O(1) "O(1)") operation.
- *Memory*: Zero overhead required. The total size of an array is the sum of all
    its items.
- *Cache Optimized*: Arrays are guaranteed to have optimal [spatial
    locality](https://en.wikipedia.org/wiki/Locality_of_reference) which can
    have profound performance implications. See the [Memory
    Cache](../memory_cache/) section for more details.

### Disadvantages
- *Insertions*: Inserting a new item changes the total size of the array.
    Because each item is stored contiguously, a new section of memory must be
    allocated to accommodate it. Next, existing items must be copied into the
    newly allocated area. The situation is exacerbated if items are inserted
    into the middle because existing items must also be shifted to the left or
    right.
- *Deletions*: Deleting is subject to the same problems as insertions. Items
    must be stored contiguously; therefore, they have to be shifted either to
    the left or right to fill the missing void.

## Linked Lists

Much like an array, a linked list is simply a collection of items. The
difference is that linked list items are not stored contiguously. Each item
maintains a pointer to the next item in the list. This is depicted graphically
below.

```
Address
0x001   { ---- next=0x010 }->       List Head
                            |
0x009   unrelated bits      |
        <--------------------
        |
0x010   { ---- next=0x018 }->
                            |
        <--------------------
        |
0x018   { ---- next=0x020 }->
                            |
        <--------------------
        |
0x020   { ---- next=0x030 }->
                            |
0x028   unrelated bits      |
        <--------------------
        |
0x030   { ---- next=NULL }          List Tail
```

### Advantages
- *Insertions*: Insertions anywhere in the list is a
    ![O(1)](https://latex.codecogs.com/gif.latex?O(1)) operation. It only
    requires updating pointers.
- *Deletions*: Deleting from the list is a
    ![O(1)](https://latex.codecogs.com/gif.latex?O(1)) operation. It only
    requires updating pointers.

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

There are essentially two types of linked lists: *singly linked lists* and
*doubly linked lists*. As the names imply, singly linked lists maintain a single
pointer to the next item. Doubly linked lists maintain a pointer to the next
item as well as a pointer the previous item. A double linked list is depicted
below.

```
                            Doubly Linked List

Address
0x001   { prev=NULL ---- next=0x010 }<->       List Head
                                       |
0x009   unrelated bits                 |
        <------------------------------>
        |
0x010   { prev=Ox001 ---- next=0x018 }<->
                                        |
        <------------------------------->
        |
0x018   { prev=Ox010 ---- next=0x020 }<->
                                        |
        <------------------------------->
        |
0x020   { prev=Ox018 ---- next=0x030 }<->
                                        |
0x028   unrelated bits                  |
        <------------------------------->
        |
0x030   { prev=x020 ---- next=NULL }          List Tail
```

Linked lists are especially helpful for things like [Stack](../stack/)s,
[Heap](../heap/)s, and [Queue](../queue/)s.

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
