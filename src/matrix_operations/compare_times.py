from matplotlib import pyplot as plt
import numpy as np
import ctypes
from enum import IntEnum
import statistics
import matplotlib
matplotlib.use('Agg')

lib = ctypes.CDLL('./algo.so')

NUM_TIME_RUNS = 3
TEST_FOR_Ns = [2 ** 5, 2 ** 7, 2 ** 8, 2 ** 9, 2 ** 10, 2 ** 11]


class CtypesEnum(IntEnum):
    """A ctypes-compatible IntEnum superclass."""
    @classmethod
    def from_param(cls, obj):
        return int(obj)


class Algo(CtypesEnum):
    NAIVE_MATRIX_MULTIPLY = 1
    TILING_MATRIX_MULTIPLY = 2
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


def generate_chart():
    full_data = []
    plt.figure(figsize=(8, 6))

    fig, ax = plt.subplots()
    bar_width = 0.20
    X = np.arange(len(TEST_FOR_Ns))

    plt.ylabel('sec')
    plt.xlabel('n')

    for algo in Algo:
        print('running {}'.format(algo.name), flush=True)

        data = []
        for n in TEST_FOR_Ns:
            time = median_run_time(lambda: lib.MatrixMultiplyTime(n, algo))
            data.append(time)

        plt.bar(X, data, bar_width, label=format_name(algo))
        # plt.plot(TEST_FOR_Ns, data, label=format_name(algo))
        full_data.append((algo, data))
        X = X + bar_width

    plt.legend()
    # plt.ticklabel_format(style='plain')
    plt.xticks(np.arange(len(TEST_FOR_Ns)) + (bar_width/2), TEST_FOR_Ns)
    plt.savefig('./run_time_data/MATRIX_MULTIPLY.png')
    plt.clf()

    print('chart created', flush=True)

    generate_md_table(TEST_FOR_Ns, full_data)


if __name__ == "__main__":
    generate_chart()
