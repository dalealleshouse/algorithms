import ctypes
import statistics
from enum import IntEnum

lib = ctypes.CDLL('./algo.so')

NUM_TIME_RUNS = 3


class CtypesEnum(IntEnum):
    """A ctypes-compatible IntEnum superclass."""
    @classmethod
    def from_param(cls, obj):
        return int(obj)


class SortingAlgo(CtypesEnum):
    C = 1,
    BUBBLE = 2,
    INSERTION = 3,
    SELECTION = 4,
    QUICK = 5,
    MERGE = 6


class ArrayType(CtypesEnum):
    RANDOM = 1,
    SORTED = 2,
    REVERSED = 3,
    DUPLICATES = 4


lib.sort_time.argtypes = [ctypes.c_size_t, ArrayType, SortingAlgo]
lib.sort_time.restype = ctypes.c_double


def median_run_time(n, array_type, algo):
    times = []

    for i in range(NUM_TIME_RUNS):
        times.append(lib.sort_time(n, array_type, algo))

    return statistics.median(times)


if __name__ == "__main__":
    time = median_run_time(100000, ArrayType.RANDOM, SortingAlgo.QUICK)
    print("Median:", "%.10f" % time)
