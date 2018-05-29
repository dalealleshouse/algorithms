CC = clang-6.0
CFLAGS = -g -Wall -Wextra -Werror -std=c11 -fsanitize=undefined

default: test

merge_sort.o: merge_sort.c merge_sort.h
	$(CC) $(CFLAGS) -c merge_sort.c

test: clean merge_sort.o
	$(CC) $(CFLAGS) -o test merge_sort_test.c merge_sort.o -lcunit
	./test

clean:
	rm -f *~ *.o test
