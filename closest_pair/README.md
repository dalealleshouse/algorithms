# Closest Pair

Given an array of points in a plane, find the pair of points that are the
closest. This is a very useful algorithm for robotics, computer vision, and
graphics.

## Runtime
O(n(log(2, n)))

## Input
An array of points with x and y coordinates

## Output
Pair of points that have the smallest Euclidean distance

## Pseudo Code

### Euclidean Distance
``` math
d(p_1, p_2) = sqrt((x_1-x_2)^2-(y_1-y_2)^2)
```

### Brute Force
``` pseudo
n = length of input array
P = array of points

min = nothing

i = 1 to n:
    j = i + 1 to n:
        if d(P[i], P[j]) < min:
            min = d(P[i], P[j])

return min
```

### Closest Pair
``` pseudo
n = length of input array
P = array of points

// Sorting is only required the first time, recursive invocations are already sorted
P_x = array of points sorted by x coordinate
P_y = array of points sorted by y coordinate

// Base case, if 3 items or less, standard brute force
if n <= 3:
    return BruteForce(n, P_x)
    
// Split the points into right and left halves
L_x = first half of P_x
R_x = last half of P_x
L_y = nothing
R_y = nothing

// In order to get the left and right arrays sorted by y coordinate, do a linear scan of the already sorted array.
// It would be possible to simply sort L_x and R_x, but that would raise the overall algorithmic complexity
// ASSUMPTION: All x coordinates are distinct.
mid_x = last element in L_x
i = 1 to n:
    if P_y[i].x <= mid_x:
        add P_y[i] to L_y
    else:
        add P_y[i] to R_y

// Recursive calls
smallest_left = ClosestPair(n/2, L_x, L_y)
smallest_right = ClosestPair(n/2, R_x, R_y)

delta = min(smallest_left, smallest_right)

smallest_split = ClosestSplitPair(n, P_x, P_y, delta)
return min(smallest_split, delta)
```

### Closest Split Pair
``` pseudo
n = length of input array
P_x = array of points sorted by x coordinate
P_y = array of points sorted by y coordinate
delta = min distance between items in P_x and P_y

// We only need the points that are less than delta from the mid point. The result is an array (S) sorted by y
x_bar = P_x[n/2] // largest coordinate in left half
S = empty array
max_x = x_bar + delta
min_x = x_bar - delta
i = 1 to n:
    if P_y[i].x > min_x and P_y[i].x < max_x:
        add P_y[i] to S

// For every item in S, check the distance of the items within 7 positions
// This is almost unbelievable but it's possible to construct a proof that shows the closest item will have an x
// coordinate within delta of x bar and be no more than 7 positions from the sorted y point.
smallest = nothing
l = length of S
i = 1 to l - 1:
    j = i + 1 to min(i + 7, l):
        if d(S[i], S[j]) < smallest:
            smallest = d(S[i], S[j])

return smallest
```
