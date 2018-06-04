def count_inversions(arr, comparator):
    if not arr:
        return arr

    inversions = 0
    length = len(arr)

    if (length == 1):
        return arr, 0

    half = int(length / 2)

    first, first_i = count_inversions(arr[:half], comparator)
    second, second_i = count_inversions(arr[half:], comparator)
    inversions += first_i + second_i

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

        if result <= 0:
            sorted_arr[x] = first[first_pos]
            first_pos += 1
        else:
            inversions += first_len - first_pos
            sorted_arr[x] = second[second_pos]
            second_pos += 1

    return sorted_arr, inversions
