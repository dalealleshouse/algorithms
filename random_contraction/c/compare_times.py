import ctypes
import sys
from enum import IntEnum
import statistics
import matplotlib
matplotlib.use('Agg')
from matplotlib import pyplot as plt  # noqa

lib = ctypes.CDLL('./algo.so')

NUM_TIME_RUNS = 3
TEST_FOR_Ns = [25, 50, 75, 100, 125]


class CtypesEnum(IntEnum):
    """A ctypes-compatible IntEnum superclass."""
    @classmethod
    def from_param(cls, obj):
        return int(obj)


class Algo(CtypesEnum):
    KARGER = 1,
    KARGER_STEIN = 2


lib.AlgoTime.restype = ctypes.c_double
lib.AlgoTime.argtypes = [ctypes.c_size_t, ctypes.c_size_t, Algo]


def median_run_time(func):
    times = []

    for i in range(NUM_TIME_RUNS):
        times.append(func())

    return statistics.median(times)


def generate_md_table(ns, data):
    n_headers = ""
    header_sep = "--|"

    for n in ns:
        n_headers += 'n={:d},m={:d} |'.format(n, n * 10)
        header_sep += "--|"

    print("|ALGORITHM|", n_headers)
    print(header_sep)

    for d in data:
        times = ""
        for v in d[1]:
            times += '{:.6f} sec|'.format(v)

        print('|{} |{}'.format(d[0].name, times))

    sys.stdout.flush()


def generate_chart():
    full_data = []
    plt.figure(figsize=(8, 6))

    plt.title("Random Contraction")
    plt.ylabel('sec')
    plt.xlabel('n')

    for algo in Algo:
        print('running {}'.format(algo.name))
        sys.stdout.flush()

        data = []
        for n in TEST_FOR_Ns:
            print('running {} {}'.format(algo.name, n))
            sys.stdout.flush()
            time = median_run_time(lambda: lib.AlgoTime(n, n * 10, algo))
            data.append(time)

        plt.plot(TEST_FOR_Ns, data, label=algo.name)
        full_data.append((algo, data))

    plt.legend()
    plt.savefig('RANDOM_CONTRACTION.png')
    plt.clf()

    print('chart created')

    generate_md_table(TEST_FOR_Ns, full_data)


if __name__ == "__main__":
    generate_chart()
