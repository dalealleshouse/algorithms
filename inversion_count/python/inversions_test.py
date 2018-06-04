from inversions import count_inversions


def test_null_does_not_throw():
    count_inversions(None, None)


def int_comparator(x, y):
    if(x == y):
        return 0

    if (x < y):
        return -1

    return 1


def test_count_single_inversion():
    expected = [1, 2, 3], 1
    arr = [1, 3, 2]

    result = count_inversions(arr, int_comparator)
    assert expected == result


def test_count_two_inversions():
    expected = [1, 2, 3, 4], 2
    arr = [2, 1, 4, 3]

    result = count_inversions(arr, int_comparator)
    assert expected == result


def test_count_reverse_sorted_array():
    expected = [1, 2, 3, 4, 5, 6], 15
    arr = [6, 5, 4, 3, 2, 1]

    result = count_inversions(arr, int_comparator)
    assert expected == result


def test_count_odd_sized_array():
    expected = [1, 2, 3, 4, 5], 10
    arr = [5, 4, 3, 2, 1]

    result = count_inversions(arr, int_comparator)
    assert expected == result


def test_count_sorted_array():
    expected = [1, 2, 3, 4, 5, 6], 0
    arr = [1, 2, 3, 4, 5, 6]

    result = count_inversions(arr, int_comparator)
    assert expected == result


def test_does_not_count_equal_items():
    expected = [1, 3, 3, 3, 5], 0
    arr = [1, 3, 3, 3, 5]

    result = count_inversions(arr, int_comparator)
    assert expected == result
