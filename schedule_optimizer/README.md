# Scheduling 
A common problem in computer science is scheduling tasks (aka jobs). For
instance: threads on a CPU, lectures in a classroom, or meetings on a calender.
Tasks typically have 2 attributes.

1. Time 
1. Weight (priority)

A schedule is the order in which each task is completed.

_Completion time_ = Sum of the lengths of the preceding jobs in a schedule
(![S](https://latex.codecogs.com/gif.latex?S)) plus the length of the job
(![j](https://latex.codecogs.com/gif.latex?j)) itself

![C+{j}(S)](https://latex.codecogs.com/gif.latex?C_{j}(S))

The _Objective Functions_ defines an "optimal schedule". For the purposes here,
optimal schedule is defined as the minimum sum of weighted completion times.

![\min\sum_{n}^{j=1}w_{j}C_{j}(S)](https://latex.codecogs.com/gif.latex?\min\sum_{n}^{j=1}w_{j}C_{j}(S))

The goal is to minimize the weighted completion time over the entire schedule.
Calculating the weighted completion time for each possible schedule would
require calculating ![n!](https://latex.codecogs.com/gif.latex?n!) different
schedules. Therefore, a greed approach is optimal in this case.

Consider the case where jobs = `[{l=5, w=10}, {l=5, w=20}, {l=5, w=5}]`
- The optimal schedule is `[{l=5, w=20}, {l=5, w=10}, {l=5, w=5}]` because it
    has the minimal sum of weighted completion times.

    Optimal Schedule:

    |                 |`{l=5, w=20}`  |`{l=5, w=10}`  |`{l=5, w=5}`   |
    |-----------------|---------------|---------------|---------------|
    |Weighted Time    |5*20 = 100     |(5+5)*10 = 100 |(10+5)*5 = 75  |
    |Objective        |100            |200            |**275**        |

    Suboptimal Schedule:

    |                 |`{l=5, w=5}`   |`{l=5, w=10}`  |`{l=5, w=20}`  |
    |-----------------|---------------|---------------|---------------|
    |Weighted Time    |5*5 = 25       |(5+5)*10 = 100 |(10+5)*20 = 300|
    |Objective        |25             |125            |**425**        |

    * The reader is encouraged to check the other four cases if they are in
      doubt

Consider the case where jobs = `[{l=20, w=5}, {l=5, w=5}, {l=10, w=5}]`
- The optimal schedule is `[{l=5, w=5}, {l=10, w=5}, {l=20, w=5}]` because it
    has the minimal sum of weighted completion times.

    Optimal Schedule:

    |                 |`{l=5, w=5}`   |`{l=10, w=5}`  |`{l=20, w=5}`  |
    |-----------------|---------------|---------------|---------------|
    |Weighted Time    |5*5 = 25       |(5+10)*5 = 75  |(15+20)*5 = 175|
    |Objective        |25             |100            |**275**        |

    Suboptimal Schedule:

    |                 |`{l=20, w=5}`  |`{l=10, w=5}`  |`{l=5, w=5}`   |
    |-----------------|---------------|---------------|---------------|
    |Weighted Time    |20*5 = 100     |(20+10)*5 = 150|(30+5)*5 = 175 |
    |Objective        |100            |250            |**425**        |

    * The reader is encouraged to check the other four cases if they are in
      doubt

## Asymptotic Complexity
![O(n\log{n})](https://latex.codecogs.com/gif.latex?O(n\log{n}))

## Pseudo Code
This is a very simple example of a greedy algorithm.

``` pseudo
schedule:
    J = set of n jobs with postive lengths and weights
    returns: job sequence that minimized the sum of weighted completion time
    order J by weight/length in descending order
```
