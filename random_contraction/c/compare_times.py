import ctypes
import sys
from enum import IntEnum
import statistics
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt #noqa

lib = ctypes.CDLL('./algo.so')

NUM_TIME_RUNS = 3
TEST_FOR_Ns = [10 ** 2, 10 ** 3, 10 ** 4, 10 ** 5, 10 ** 6]


class CtypesEnum(IntEnum):
    """A ctypes-compatible IntEnum superclass."""
    @classmethod
    def from_param(cls, obj):
        return int(obj)


class Algo(CtypesEnum):
    QUICK_SELECT = 1,
    SORT_SELECT = 2,


class LinearAlgo(CtypesEnum):
    LINEAR_SCAN_MAX = 1,
    LINEAR_SCAN_MIN = 2


lib.select_time.argtypes = [ctypes.c_size_t, Algo]
lib.select_time.restype = ctypes.c_double
lib.linear_time.argtypes = [ctypes.c_size_t, LinearAlgo]
lib.linear_time.restype = ctypes.c_double


def median_run_time(func):
    times = []

    for i in range(NUM_TIME_RUNS):
        times.append(func())

    return statistics.median(times)


def generate_md_table(ns, data):
    n_headers = ""
    header_sep = "--|"

    for n in ns:
        n_headers += 'n={:d} |'.format(n)
        header_sep += "--|"

    print("|ALGORITHM|", n_headers)
    print(header_sep)

    for d in data:
        times = ""
        for v in d[1]:
            times += '{:.6f} sec|'.format(v)

        print('|{} |{}'.format(d[0].name, times))

    sys.stdout.flush()


def get_title(nth):
    if nth == 1:
        return 'Find the Minimum Value'
    if nth == -1:
        return 'Find the Median Value'
    else:
        return 'Find the {}th Value'.format(nth)


def generate_chart(nth):
    full_data = []
    plt.figure(figsize=(8, 6))

    plt.title(get_title(nth))
    plt.ylabel('sec')
    plt.xlabel('n')

    for algo in Algo:
        print('running {} {}'.format(algo.name, nth))
        sys.stdout.flush()

        data = []
        for n in TEST_FOR_Ns:
            this_nth = nth

            if nth == -1:
                this_nth = n / 2

            time = median_run_time(lambda: lib.select_time(n, this_nth, algo))
            data.append(time)

        plt.plot(TEST_FOR_Ns, data, label=algo.name)
        full_data.append((algo, data))

    if nth == 1:
        print('running ', LinearAlgo.LINEAR_SCAN_MIN.name)
        sys.stdout.flush()
        min_algo = LinearAlgo.LINEAR_SCAN_MIN

        data = []
        for n in TEST_FOR_Ns:
            time = median_run_time(lambda: lib.linear_time(n, min_algo))
            data.append(time)

        plt.plot(TEST_FOR_Ns, data, label=min_algo.name)
        full_data.append((min_algo, data))

    plt.legend()
    plt.savefig('QUICK_SELECT-' + str(nth) + '.png')
    plt.clf()

    print('chart created')

    generate_md_table(TEST_FOR_Ns, full_data)


if __name__ == "__main__":
    generate_chart(5)
    generate_chart(1)
    generate_chart(-1)
