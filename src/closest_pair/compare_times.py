from matplotlib import pyplot as plt
import numpy as np
import ctypes
from enum import IntEnum
import statistics
import matplotlib
matplotlib.use('Agg')

lib = ctypes.CDLL('./algo.so')

FILE_NAME = 'CLOSEST-PAIR'
NUM_TIME_RUNS = 3
Ns = [10 ** 2, 10 ** 3, 10 ** 4, 10 ** 5]


class CtypesEnum(IntEnum):
    """A ctypes-compatible IntEnum superclass."""
    @classmethod
    def from_param(cls, obj):
        return int(obj)


class Algo(CtypesEnum):
    NAIVE = 1,
    DIVIDE_AND_CONQUER = 2


class ChartType(CtypesEnum):
    LINE = 1,
    BAR = 2


lib.ClosestPairTime.argtypes = [ctypes.c_size_t, Algo]
lib.ClosestPairTime.restype = ctypes.c_double


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


def generate_chart(ns, chart_type):
    full_data = []
    plt.figure(figsize=(8, 6))

    bar_width = 0.15
    n_count = len(ns)
    X = np.arange(n_count)

    plt.ticklabel_format(style='plain')
    plt.ylabel('sec')
    plt.xlabel('n')

    for algo in Algo:
        print('running {}'.format(algo.name), flush=True)

        data = []
        for n in ns:
            time = median_run_time(lambda: lib.ClosestPairTime(n, algo))
            data.append(time)

        if chart_type == ChartType.LINE:
            plt.plot(ns, data, label=format_name(algo))
        elif chart_type == ChartType.BAR:
            plt.bar(X, data, bar_width, label=format_name(algo))
            X = X + bar_width

        full_data.append((algo, data))

    plt.legend()

    if chart_type == ChartType.BAR:
        plt.xticks(np.arange(n_count) + (bar_width * n_count) / 2 - bar_width,
                   ns)

    plt.savefig(f'./run_time_data/{FILE_NAME}-{chart_type.name}.png')
    plt.clf()

    print('chart created', flush=True)
    generate_md_table(ns, full_data)


if __name__ == "__main__":
    generate_chart(Ns, ChartType.LINE)
    # generate_chart(Ns, ChartType.BAR)
