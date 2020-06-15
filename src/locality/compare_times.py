import ctypes
from enum import IntEnum

lib = ctypes.CDLL('./algo.so')


class CtypesEnum(IntEnum):
    """A ctypes-compatible IntEnum superclass."""
    @classmethod
    def from_param(cls, obj):
        return int(obj)


class IterationType(CtypesEnum):
    kColumnOrder = 1,
    kRowOrder = 2,


lib.EnumerateMultiDimArray.argtypes = [ctypes.c_size_t, IterationType]
lib.EnumerateMultiDimArray.restype = ctypes.c_double


def calculate_runtime(n):
    print('enumerating a {:,} X {:,} array in row order'.format(n, n))
    time = lib.EnumerateMultiDimArray(n, IterationType.kRowOrder)
    print('completed in {:.6f} sec'.format(time))

    print()

    print('enumerating a {:,} X {:,} array in column order'.format(n, n))
    time = lib.EnumerateMultiDimArray(n, IterationType.kColumnOrder)
    print('completed in {:.6f} sec'.format(time))


if __name__ == "__main__":
    calculate_runtime(100000000000)
