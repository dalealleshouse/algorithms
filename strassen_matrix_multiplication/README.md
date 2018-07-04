# Strassen Matrix Multiplication

Matrix multiplication algorithm. The simple and obvious implementation is O(n^3). Strassen improves on this by
eliminating a recursive call.

Although this is more efficient in terms of algorithmic complexity, it's actually less performant for anything expect
extremely large matrices because of the high number of constant operations.

Using C, the largest matrix that can be multiplied using the recursive Strassen algorithm without a stack overflow is
256 X 256. Strassen runs slower than the standard brute force implementation for all practical test cases. Therefore
the utility of this algorithm isn't immediately obvious. Further tests using either a loop or tail call optimized
language are required.

Click [here](https://en.wikipedia.org/wiki/Matrix_multiplication) for a refresher on matrix multiplication.

## Runtime
O(n^2.81)

## Input
- Matrix A
- Matrix B

## Output
Product of A and B

## Pseudo Code
Assumptions
- matrices are square
- size of matrices is a power of 2


``` psudo
n = size of matrices
A = matrix 1
B = matrix 2

if n == 1
    return A X B

split matrices in quadrants and recursively multiply

    a | b       e | f
A = -----   B = -----
    c | d       g | h

p1 = a(f-h)
p2 = (a+b)h
p3 = (c+d)e
p4 = d(g-e)
p5 = (a+d)(e+h)
p6 = (b-d)(g+h)
p7 = (a-c)(e+f)

        p5+p4-p2+p6 | p1+p2
return  --------------------------
        p3+p4       | p1+p5-p3-p7

```
