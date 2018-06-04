CC = clang-6.0
CFLAGS = -O3 -Wall -Wextra -Werror -std=c11 -fsanitize=undefined -pedantic-errors

default: test

inversions.o: inversions.c inversions.h
	$(CC) $(CFLAGS) -c inversions.c

test: clean inversions.o
	$(CC) $(CFLAGS) -o test inversions_test.c inversions.o -lcunit
	./test

clean:
	rm -f *~ *.o test
