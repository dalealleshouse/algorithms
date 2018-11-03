# Stack (Last-In-First-Out)
#data_structure

Stacks are highly useful and very powerful.  Most mainstream languages come
equipped with a stack construct. With the exception of C programmers, it's
highly unlikely that the average coder will every have to implement a stack.
However, this does not preclude the need for understanding how they work.

At a base level, a stack is simply a [list data
structure](../list_data_structures) with a few specialized methods that make it
uniquely suited for some programming tasks. These methods are `push`, which adds
an item to the head of the list and `pop` which removes and returns the item in
the head position. The most common analogy is a stack of plates. You place a
plate on top of the stack, and you must retrieve the last plate placed on the
stack before you can access plates below it. In software parlance, a stack is
known as a *Last-in-First-Out* data structure because the last item pushed on
the stack is the first one out (clever, huh?).

Typically, a [Linked List](../list_data_structures/README.md#linked-lists) is
used when implementing a stack. However, an
[Array](../list_data_structures/README.md#arrays) is also a viable option. The
reader is encouraged to examine the code to get a better understanding of how
stacks work.

## Pseudo Code
Typical methods

```
push:
    Adds an item to the head of the list

pop:
    Remove item from the head the list and returns it
```

## Applications

Below are just a couple examples of common uses of stacks.

* Memory Management: Operating systems use a stack for managing memory.
* Backtracking: Keep track of tasks so they can be undone if necessary. Think of
    what happens when you press `ctrl+z` while using MS Word, or `u` when using
    VIM.
* Graph [Depth First Search](../graph_search/README.md#depth-first-search-dfs)
