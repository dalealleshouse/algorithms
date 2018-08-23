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

Must like an array, a linked list is simply a collection of items. The
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

1. Insertion
2. Sequential Access
3. Random Access
