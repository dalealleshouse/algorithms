# Queue
#data_structure

## Queue (First-In-First-Out)
Queues are highly useful and very powerful.  Most mainstream languages come
equipped with a queue construct. With the exception of C programmers, it's
highly unlikely that the average coder will every have to implement a queue.
However, this does not preclude the need for understanding how they work.

At a base level, a queue is simply a [list data
structure](../list_data_structures) with a few specialized methods that make it
uniquely suited for some programming tasks. These methods are `enqueue`, which
adds an item to the end of the list and `dequeue` which removes and returns the
item in the head position. Think of a queue like a line to get into a night
club. New people are added to the end of the queue and people are removed from
the front of the queue as they are let in to enjoy a night of dancing. In
software parlance, a queue is known as a *First-in-First-Out* data structure
because the first item added to the queue is the first one out (clever, huh?).

Typically, a [Linked List](../list_data_structures/README.md#linked-lists) is
used when implementing a queue. However, an
[Array](../list_data_structures/README.md#arrays) is also a viable option. The
reader is encouraged to examine the code to get a better understanding of how
queues work.

### Asymptotic Time Complexity
* Enqueue: ![O(1)](https://latex.codecogs.com/gif.latex?O(1) "O(1)")
* Dequeue: ![O(1)](https://latex.codecogs.com/gif.latex?O(1) "O(1)")

### Pseudo Code
Typical methods

```
enqueue:
    Add item to the end of the list

dequeue:
    Remove item from the head of the list and returns it

peek:
    Return item from the head without removing it from the list
```

### Applications

Below are just a couple examples of common uses for queueing.

* Job Scheduling: A print driver maintains a queue of print jobs and sends them
    to the printer as it is available.
* Buffering: Streaming video and audio applications typically maintain a buffer
    of content to compensate for erratic connections.
* CPU Scheduling: A thread can place a job on a queue to be picked by another
    thread. *Caveat*: This requires the queue to be synchronized.
    Multi-threading concepts are covered in other sections.

## Priority Queue
Another type of queue is a *Priority Queue*. These behave very similarly to
standard Queues with the exception that rather than being *First-In-First-Out*,
the order in which items are removed is determined by a predefined priority
function. There are multiple ways to implement this. A somewhat slow
implementation using [Linked
Lists](../list_data_structures/README.md#linked-list) is shown below. It is
unlikely that anyone would use such an implementation in a production scenario;
however, it is a useful learning tool for understanding how Priority Queues work
at a conceptual level.

### Asymptotic Time Complexity
* Insert: ![O(n)](https://latex.codecogs.com/gif.latex?O(n) "O(n)")
* Remove: ![O(1)](https://latex.codecogs.com/gif.latex?O(1) "O(1)")

### Pseudo Code

```
priority_function:
    item1
    item2

    if item1 should be before item2
        return 1
    else
        return 0


insert:
    new_item

    for each item in the list:
        if prority_function(new_item, item) = 1
            insert new_item before item
            break out of loop

remove:
    Remove item from the head of the list and returns it

peek:
    Return item from the head without removing it from the list
```

