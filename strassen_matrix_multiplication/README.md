# Strassen Matrix Multiplication

Efficient matrix multiplication algorithm. The simple and obvious
implementation is O(n^3). Strassen improves on this by eliminating a recursive
call.

Although this is more efficient in terms of algorithmic complexity, it's
actually less performant in many instances because of the high number of
constant operations.

This is an example of a divide and conquer algorithm.

Click [here](https://en.wikipedia.org/wiki/Matrix_multiplication) for a
refresher on matrix multiplication.

## Runtime
O(n^2)

## Input
- Matrix A and Matrix B

## Output
Product of A and B

## Pseudo Code

```
A = matrix 1
B = matrix 2
```
