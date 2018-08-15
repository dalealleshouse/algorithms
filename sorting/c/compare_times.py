import ctypes
import sys
from enum import IntEnum
import statistics
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt #noqa

lib = ctypes.CDLL('./algo.so')

NUM_TIME_RUNS = 3
TEST_FOR_Ns = [10 ** 2, 10 ** 3, 10 ** 4, 10 ** 5]


class CtypesEnum(IntEnum):
    """A ctypes-compatible IntEnum superclass."""
    @classmethod
    def from_param(cls, obj):
        return int(obj)


class SortingAlgo(CtypesEnum):
    C_QSORT = 1,
    BUBBLE = 2,
    INSERTION = 3,
    SELECTION = 4,
    QUICK = 5,
    MERGE = 6
    QUICK_PIVOT_RANDOM = 7,
    QUICK_PIVOT_MEDIAN = 8,
    QUICK_PIVOT_FIRST = 9,
    QUICK_PIVOT_LAST = 10


class ArrayType(CtypesEnum):
    RANDOM = 1,
    SORTED = 2,
    REVERSED = 3,
    # DUPLICATES = 4


lib.sort_time.argtypes = [ctypes.c_size_t, ArrayType, SortingAlgo]
lib.sort_time.restype = ctypes.c_double


def median_run_time(n, array_type, algo):
    times = []

    for i in range(NUM_TIME_RUNS):
        times.append(lib.sort_time(n, array_type, algo))

    return statistics.median(times)


def generate_md_table(ns, data, arr_type):
    f = open("run_results.txt", "a+")
    f.write(arr_type.name)
    f.write("\n")

    n_headers = ""
    header_sep = "|--|"

    for n in ns:
        n_headers += 'n={:d} |'.format(n)
        header_sep += "--|"

    f.write("|ALGORITHM|")
    f.write(n_headers)
    f.write("\n")
    f.write(header_sep)
    f.write("\n")

    for d in data:
        times = ""
        for v in d[1]:
            times += '{:.6f} sec|'.format(v)

        f.write('|{} |{}'.format(d[0].name, times))
        f.write("\n")


def generate_chart(arr_type, algos, save_as):
    print("generating " + arr_type.name)
    sys.stdout.flush()

    full_data = []
    plt.figure(figsize=(8, 6))
    plt.title(arr_type.name + ' ARRAY')
    plt.ylabel('sec')
    plt.xlabel('n')

    for algo in algos:
        data = []
        for n in TEST_FOR_Ns:
            time = median_run_time(n, arr_type, algo)
            data.append(time)

        plt.plot(TEST_FOR_Ns, data, label=algo.name)
        full_data.append((algo, data))

    plt.legend()
    plt.savefig('{}{}.png'.format(save_as, arr_type.name))
    plt.clf()

    print('chart created')

    generate_md_table(TEST_FOR_Ns, full_data, arr_type)


if __name__ == "__main__":
    for arr_type in ArrayType:
        generate_chart(arr_type, [SortingAlgo.C_QSORT, SortingAlgo.BUBBLE,
                                  SortingAlgo.INSERTION, SortingAlgo.SELECTION,
                                  SortingAlgo.QUICK, SortingAlgo.MERGE],
                       "")

        generate_chart(arr_type, [SortingAlgo.QUICK_PIVOT_LAST,
                                  SortingAlgo.QUICK_PIVOT_FIRST,
                                  SortingAlgo.QUICK_PIVOT_RANDOM,
                                  SortingAlgo.QUICK_PIVOT_MEDIAN],
                       "PIVOT-")
