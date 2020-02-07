# Code Standards

The C code included in this repository should comply with the following
standards.

## Function

Every function should return a Result code indicating it's status

``` c
typedef enum Result {
    NotFound = -8,
    Empty = -7,
    DependancyError = -6,
    ArgumentOutOfRange = -5,
    InvalidIndex = -4,
    ArithmeticOverflow = -3,
    FailedMemoryAllocation = -2,
    NullParameter = -1,
    Success = 0
} Result;

Result someFunction();
```

Any additional output should be returned via an output parameter, that should be
placed as the last function parameter.

```c
Result someFunction(const int par1, const float par2, struct *returnValue);
```

