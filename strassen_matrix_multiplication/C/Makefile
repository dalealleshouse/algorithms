CC = clang-6.0
CFLAGS = -O3 -Wall -Wextra -Werror -std=c11 -fsanitize=undefined -pedantic-errors

default: test

strassen.o: strassen.c strassen.h
	$(CC) $(CFLAGS) -c strassen.c

test: clean strassen.o
	$(CC) $(CFLAGS) -o test strassen_test.c strassen.o -lcunit
	./test

clean:
	rm -f *~ *.o test
