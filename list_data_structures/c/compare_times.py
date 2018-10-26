import ctypes
from enum import IntEnum
import statistics
import os
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


class Operations(CtypesEnum):
    INSERT = 1,
    SEARCH = 2,
    ENUMERATE = 3


class Structures(CtypesEnum):
    ARRAY = 1,
    LINKED_LIST = 2,
    LINKED_LIST_POOR_LOCALITY = 3,
    BINARY_TREE = 4,
    BINARY_TREE_UNBALANCED = 5


lib.OperationTime.argtypes = [Operations, Structures, ctypes.c_size_t]
lib.OperationTime.restype = ctypes.c_double


def median_run_time(func):
    times = []

    for i in range(NUM_TIME_RUNS):
        times.append(func())

    return statistics.median(times)


def generate_md_table(ns, op, data):
    f = open("run_results.txt", "a+")
    f.write(op.name)
    f.write(os.linesep)
    n_headers = ""
    header_sep = "|--|"

    for n in ns:
        n_headers += 'n={:d} |'.format(n)
        header_sep += "--|"

    f.write("|STRUCTURE|")
    f.write(n_headers)
    f.write(os.linesep)

    f.write(header_sep)
    f.write("\n")

    for d in data:
        times = ""
        for v in d[1]:
            times += '{:.6f} sec|'.format(v)

        f.write('|{} |{}'.format(d[0].name, times))
        f.write("\n")

    f.write(os.linesep)
    f.write(os.linesep)
    f.write(os.linesep)


def generate_chart(op, structs):
    full_data = []
    plt.figure(figsize=(8, 6))

    plt.title(op.name)
    plt.ylabel('sec')
    plt.xlabel('n')

    print()
    print(op.name)

    for st in structs:
        data = []
        for n in TEST_FOR_Ns:
            time = median_run_time(lambda: lib.OperationTime(op, st, n))
            data.append(time)

        plt.plot(TEST_FOR_Ns, data, label=st.name)
        full_data.append((st, data))

    plt.legend()
    plt.savefig(op.name + '.png')
    plt.clf()

    generate_md_table(TEST_FOR_Ns, op, full_data)
    print()
    print()


if __name__ == "__main__":
    generate_chart(Operations.INSERT,
                   [Structures.ARRAY, Structures.LINKED_LIST,
                    Structures.BINARY_TREE])
    generate_chart(Operations.SEARCH,
                   [Structures.ARRAY, Structures.LINKED_LIST,
                    Structures.LINKED_LIST_POOR_LOCALITY,
                    Structures.BINARY_TREE, Structures.BINARY_TREE_UNBALANCED])
    generate_chart(Operations.ENUMERATE,
                   [Structures.ARRAY, Structures.LINKED_LIST,
                    Structures.LINKED_LIST_POOR_LOCALITY,
                    Structures.BINARY_TREE, Structures.BINARY_TREE_UNBALANCED])
