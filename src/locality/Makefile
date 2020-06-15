SHELL 	= /bin/sh
CC 		= clang-10

CFLAGS       = -pedantic-errors -Wall -Wextra -Werror -Wthread-safety -D_POSIX_C_SOURCE=200809L
SHAREDFLAGS	 = -std=c11 -O3 -D NDEBUG -fPIC -shared
LINKFLAGS 	 = -lcunit -flto 

SHARED_TARGET	= algo.so
SOURCES			= $(shell echo *.c )
OBJECTS			= $(SOURCES:.c=.o)

shared: $(SOURCES)
	$(CC) $(CFLAGS) $(SHAREDFLAGS) -o $(SHARED_TARGET) $(SOURCES) $(LINKFLAGS)

%.o: %.c
	  $(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ $< -flto
