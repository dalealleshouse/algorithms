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


lib.SelectTime.argtypes = [ctypes.c_size_t, Algo]
lib.SelectTime.restype = ctypes.c_double

def format_name(enum_val):
    return enum_val.name.replace('_', ' ').title()


def get_title(nth):
    if nth == 1:
        return 'Find the Minimum Value'
    if nth == -1:
        return 'Find the Median Value'
    else:
        return 'Find the {}th Value'.format(nth)


def median_run_time(func):
    times = []

    for i in range(NUM_TIME_RUNS):
        times.append(func())

    return statistics.median(times)


def generate_md_table(ns, data, nth):
    f = open("run_time_data/run_results.txt", "a+")
    f.write("\n")
    f.write(get_title(nth))
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


def generate_chart(nth):
    full_data = []
    plt.figure(figsize=(8, 6))

    plt.title(get_title(nth))
    plt.ylabel('sec')
    plt.xlabel('n')

    for algo in Algo:
        print('running {} {}'.format(algo.name, nth), flush=True)

        data = []
        for n in TEST_FOR_Ns:
            this_nth = nth

            if nth == -1:
                this_nth = n / 2

            time = median_run_time(lambda: lib.SelectTime(n, this_nth, algo))
            data.append(time)

        plt.plot(TEST_FOR_Ns, data, label=format_name(algo))
        full_data.append((algo, data))

    plt.legend()
    plt.savefig('./run_time_data/QUICK_SELECT-' + str(nth) + '.png')
    plt.clf()

    print('chart created')

    generate_md_table(TEST_FOR_Ns, full_data, nth)


if __name__ == "__main__":
    generate_chart(5)
    generate_chart(1)
    generate_chart(-1)
