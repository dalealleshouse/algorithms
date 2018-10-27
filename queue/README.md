# Queue (First-In-First-Out)
#data_structure

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
used when implementing a queue. However, an array is also a viable option. The
reader is encouraged to examine the code to get a better understanding of how
queues work.

## Pseudo Code
Typical methods

```
enqueue:
    Add item to the end of the list

dequeue:
    Remove item from the head of the front of the list and return it

is_empty:
    return true if there are no items in the queue
```

## Applications

Below are just a couple examples of common uses for queueing.

* Job Scheduling: A print driver maintains a queue of print jobs and sends them
    to the printer as it is available.
* Buffering: Streaming video and audio applications typically maintain a buffer
    of content to compensate for erratic connections.
* CPU Scheduling: A thread can place a job on a queue to be picked by another
    thread. *Caveat*: This requires the queue to be synchronized.
    Multi-threading concepts are covered in other sections.
