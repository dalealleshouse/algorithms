from matplotlib import pyplot as plt
import numpy as np
import ctypes
from enum import IntEnum
import statistics
import matplotlib
matplotlib.use('Agg')

lib = ctypes.CDLL('./algo.so')

NUM_TIME_RUNS = 3
SMALL_Ns = [2 ** 4, 2 ** 5, 2 ** 6, 2 ** 7, 2 ** 8, 2 ** 9]
LARGE_Ns = [2 ** 10, 2 ** 11]


class CtypesEnum(IntEnum):
    """A ctypes-compatible IntEnum superclass."""
    @classmethod
    def from_param(cls, obj):
        return int(obj)


class Algo(CtypesEnum):
    NAIVE_MATRIX_MULTIPLY = 1
    TILING_MATRIX_MULTIPLY = 2
    TRANSPOSE_MATRIX_MULTIPLY = 4
    RECURSIVE_MATRIX_MULTIPLY = 5
    STRASSEN_MATRIX_MULTIPLY = 3


lib.MatrixMultiplyTime.argtypes = [ctypes.c_size_t, Algo]
lib.MatrixMultiplyTime.restype = ctypes.c_double


def format_name(enum_val):
    return enum_val.name.replace('_', ' ').title()


def median_run_time(func):
    times = []

    for i in range(NUM_TIME_RUNS):
        times.append(func())

    return statistics.median(times)


def generate_md_table(ns, data):
    f = open("run_time_data/run_results.txt", "a+")
    f.write("\n")

    n_headers = ""
    header_sep = "--|"

    for n in ns:
        n_headers += 'n={:d} |'.format(n)
        header_sep += "--|"

    f.write("|Algorithm|")
    f.write(n_headers)
    f.write("\n")
    f.write(header_sep)
    f.write("\n")

    for d in data:
        times = ""
        for v in d[1]:
            times += '{:.6f} sec|'.format(v)

        f.write(f'|{format_name(d[0])} |{times}')
        f.write("\n")


def generate_chart(ns, file_name):
    full_data = []
    plt.figure(figsize=(8, 6))

    bar_width = 0.15
    n_count = len(ns)
    X = np.arange(n_count)

    plt.ylabel('sec')
    plt.xlabel('n')

    for algo in Algo:
        print('running {}'.format(algo.name), flush=True)

        data = []
        for n in ns:
            time = median_run_time(lambda: lib.MatrixMultiplyTime(n, algo))
            data.append(time)

        plt.bar(X, data, bar_width, label=format_name(algo))
        full_data.append((algo, data))
        X = X + bar_width

    plt.legend()
    plt.xticks(np.arange(n_count) + (bar_width * n_count) / 2 - bar_width, ns)
    plt.savefig(f'./run_time_data/MATRIX_MULTIPLY-{file_name}.png')
    plt.clf()

    print('chart created', flush=True)
    generate_md_table(ns, full_data)


if __name__ == "__main__":
    generate_chart(SMALL_Ns, 'SMALL')
    generate_chart(LARGE_Ns, 'LARGE')
