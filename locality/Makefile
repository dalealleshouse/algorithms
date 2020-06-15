SHELL 	= /bin/sh
CC 		= clang-10

FLAGS        = -std=c11 -fsanitize=cfi -fvisibility=hidden -D_POSIX_C_SOURCE=200809L
CFLAGS       = -pedantic-errors -Wall -Wextra -Werror -Wthread-safety -D_POSIX_C_SOURCE=200809L
RELEASEFLAGS = -O3 -fsanitize=safe-stack -D NDEBUG
LINKFLAGS 	 = -flto 

TARGET			= locality_demo
SOURCES			= $(shell echo *.c )
OBJECTS			= $(SOURCES:.c=.o)

release: 
	$(CC) $(FLAGS) $(CFLAGS) $(RELEASEFLAGS) -o row row.c $(LINKFLAGS)
	$(CC) $(FLAGS) $(CFLAGS) $(RELEASEFLAGS) -o column column.c $(LINKFLAGS)

%.o: %.c
	  $(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ $< -flto
