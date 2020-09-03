"""
/*******************************************************************************
* Copyright (C) 2020 Dale Alleshouse (AKA Hideous Humpback Freak)
*  dale@alleshouse.net https://hideoushumpbackfreak.com/
*
* This file is subject to the terms and conditions defined in the 'LICENSE'
* file, which is part of this source code package.
******************************************************************************/
"""
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
    ENUMERATE = 3,
    MAX = 4,
    PREDECESSOR = 5,
    RANK = 6,
    INSERT_AT_TAIL = 7


class Structures(CtypesEnum):
    ARRAY = 1,
    SORTED_ARRAY = 2,
    LINKED_LIST = 3,
    LINKED_LIST_POOR_LOCALITY = 4,
    BINARY_TREE = 5,
    BINARY_TREE_UNBALANCED = 6,
    RED_BLACK_TREE = 7


lib.ListDataStructures_OperationTime.argtypes = [Operations, Structures,
                                                 ctypes.c_size_t]
lib.ListDataStructures_OperationTime.restype = ctypes.c_double


def median_run_time(func):
    times = []

    for i in range(NUM_TIME_RUNS):
        times.append(func())

    return statistics.median(times)


def generate_md_table(ns, op, data):
    f = open(f'run_time_data/list_data_struct_{op.name}.txt', "a+")
    f.write(op.name)
    f.write(os.linesep)
    n_headers = ""
    header_sep = "|--|"

    for n in ns:
        n_headers += 'n={:d} |'.format(n)
        header_sep += "--|"

    f.write("|Structure|")
    f.write(n_headers)
    f.write(os.linesep)

    f.write(header_sep)
    f.write("\n")

    for d in data:
        times = ""
        for v in d[1]:
            times += '{:.6f} sec|'.format(v)

        f.write('|{} |{}'.format(d[0].name.title().replace('_', ' '), times))
        f.write("\n")

    f.write(os.linesep)
    f.write(os.linesep)
    f.write(os.linesep)


def generate_chart(op, structs):
    full_data = []
    plt.figure(figsize=(8, 6))

    op_name = op.name.title().replace('_', ' ')
    plt.title(op_name)
    plt.ylabel('sec')
    plt.xlabel('n')

    print()
    print(op.name)

    for st in structs:
        st_name = st.name.title().replace('_', ' ')
        data = []
        for n in TEST_FOR_Ns:
            time = median_run_time(lambda:
                                   lib.ListDataStructures_OperationTime(op, st,
                                                                        n))
            data.append(time)

        plt.plot(TEST_FOR_Ns, data, label=st_name)
        full_data.append((st, data))

    plt.legend()
    plt.savefig('run_time_data/' + op.name + '.png')
    plt.clf()

    generate_md_table(TEST_FOR_Ns, op, full_data)
    print()
    print()


if __name__ == "__main__":
    generate_chart(Operations.INSERT, [
        Structures.ARRAY,
        Structures.LINKED_LIST,
        Structures.BINARY_TREE,
        Structures.BINARY_TREE_UNBALANCED,
        Structures.RED_BLACK_TREE])
    generate_chart(Operations.INSERT_AT_TAIL, [
        Structures.ARRAY,
        Structures.LINKED_LIST,
        Structures.BINARY_TREE,
        Structures.RED_BLACK_TREE])
    generate_chart(Operations.SEARCH, [
        Structures.ARRAY,
        Structures.SORTED_ARRAY,
        Structures.LINKED_LIST,
        Structures.LINKED_LIST_POOR_LOCALITY,
        Structures.BINARY_TREE,
        Structures.BINARY_TREE_UNBALANCED,
        Structures.RED_BLACK_TREE])
    generate_chart(Operations.ENUMERATE, [
        Structures.ARRAY,
        Structures.SORTED_ARRAY,
        Structures.LINKED_LIST,
        Structures.LINKED_LIST_POOR_LOCALITY,
        Structures.BINARY_TREE,
        Structures.BINARY_TREE_UNBALANCED,
        Structures.RED_BLACK_TREE])
    generate_chart(Operations.MAX, [
        Structures.ARRAY,
        Structures.SORTED_ARRAY,
        Structures.LINKED_LIST,
        Structures.LINKED_LIST_POOR_LOCALITY,
        Structures.BINARY_TREE,
        Structures.BINARY_TREE_UNBALANCED,
        Structures.RED_BLACK_TREE])
    generate_chart(Operations.PREDECESSOR, [
        Structures.ARRAY,
        Structures.SORTED_ARRAY,
        Structures.LINKED_LIST,
        Structures.LINKED_LIST_POOR_LOCALITY,
        Structures.BINARY_TREE,
        Structures.BINARY_TREE_UNBALANCED,
        Structures.RED_BLACK_TREE])
    generate_chart(Operations.RANK, [
        Structures.ARRAY,
        Structures.SORTED_ARRAY,
        Structures.LINKED_LIST,
        Structures.LINKED_LIST_POOR_LOCALITY,
        Structures.BINARY_TREE,
        Structures.BINARY_TREE_UNBALANCED,
        Structures.RED_BLACK_TREE])
