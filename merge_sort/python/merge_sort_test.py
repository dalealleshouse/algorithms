import random
from merge_sort import sort


def test_null_does_not_throw():
    sort(None, None)


def int_comparator(x, y):
    if(x == y):
        return 0

    if (x < y):
        return -1

    return 1


def test_sorts_even_size_array():
    expected = [1, 2, 3, 4, 5, 6]
    arr = [6, 5, 4, 3, 2, 1]
    result = sort(arr, int_comparator)
    assert expected == result


def test_sorts_pre_sorted_array():
    expected = [1, 2, 3, 4, 5, 6]
    arr = [1, 2, 3, 4, 5, 6]
    result = sort(arr, int_comparator)
    assert expected == result


def test_sorts_odd_sized_array():
    expected = [1, 2, 3, 4, 5]
    arr = [5, 4, 3, 2, 1]
    result = sort(arr, int_comparator)
    assert expected == result


def test_sorts_large_array():
    size = 100000
    arr = []

    for x in range(size):
        arr.append(random.randint(1, 50000000))

    result = sort(arr, int_comparator)

    prev = -1
    for x in result:
        assert prev <= x
        prev = x
