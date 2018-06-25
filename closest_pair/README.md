# Closest Pair

Given an array of points in a plane, find the pair of points that are the
closest. This is a very useful algorithm for robotics, computer vision, and
graphics.

## Runtime
O(n(log(2, n)))

## Input
Array of points with x and y coordinates

## Output
Pair of points that have the smallest Euclidean distance

## Pseudo Code

### Euclidean Distance
``` math
d(p_1, p_2) = sqrt((x_1-x_2)^2-(y_1-y_2)^2)
```
