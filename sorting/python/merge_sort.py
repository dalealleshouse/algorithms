def sort(arr, comparator):
    if not arr:
        return arr

    length = len(arr)

    if (length == 1):
        return arr

    half = int(length / 2)

    first = sort(arr[half:], comparator)
    second = sort(arr[:half], comparator)

    first_len = len(first)
    first_pos = 0

    second_len = len(second)
    second_pos = 0

    sorted_arr = [None] * length
    for x in range(length):
        if first_pos >= first_len:
            sorted_arr[x] = second[second_pos]
            second_pos += 1
            continue

        if second_pos >= second_len:
            sorted_arr[x] = first[first_pos]
            first_pos += 1
            continue

        result = comparator(first[first_pos], second[second_pos])

        if result < 0:
            sorted_arr[x] = first[first_pos]
            first_pos += 1
        else:
            sorted_arr[x] = second[second_pos]
            second_pos += 1

    return sorted_arr
