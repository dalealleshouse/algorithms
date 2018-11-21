SHELL 	= /bin/sh
CC 		= clang-6.0

FLAGS        = -std=c11 -fsanitize=cfi -fvisibility=hidden
CFLAGS       = -pedantic-errors -Wall -Wextra -Werror -Wthread-safety
DEBUGFLAGS   = -O0 -Wno-unused -Wno-unused-parameter -fno-omit-frame-pointer -fno-sanitize-recover=all -g -D _DEBUG
RELEASEFLAGS = -O3 -fsanitize=safe-stack -D NDEBUG
SHAREDFLAGS	 = -std=c11 -O3 -D NDEBUG -fPIC -shared
LIBFLAGS	 = -std=c11 -O3 -D NDEBUG -fvisibility=hidden
LINKFLAGS 	 = -lcunit -flto -lm src/include/libutils.a src/include/libstack.a src/include/libqueue.a src/include/libgraph_search.a

TARGET			= src/test_runner
SHARED_TARGET	= src/algo.so
LIB_NAME	 	?=utils
SOURCES			= $(shell echo src/*.c )
OBJECTS			= $(SOURCES:.c=.o)

all: $(TARGET)
all: CFLAGS += -fsanitize=safe-stack,undefined

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) $(OBJECTS) $(LINKFLAGS) -o $(TARGET) 
	ASAN_OPTIONS=detect_leaks=1 ./$(TARGET)

release: $(SOURCES)
	$(CC) $(FLAGS) $(CFLAGS) $(RELEASEFLAGS) -o $(TARGET) $(SOURCES) $(LINKFLAGS)
	./$(TARGET)

shared: $(SOURCES)
	$(CC) $(CFLAGS) $(SHAREDFLAGS) -o $(SHARED_TARGET) $(SOURCES) $(LINKFLAGS)

profile: CFLAGS += -pg
profile: $(TARGET)

code-coverage: CFLAGS += -fprofile-arcs -ftest-coverage
code-coverage: $(TARGET)

memory-san: CFLAGS += -fsanitize=memory,undefined -fsanitize-memory-track-origins
memory-san: $(TARGET)

thread-san: CFLAGS += -fsanitize=thread,undefined
thread-san: $(TARGET)

address-san: CFLAGS += -fsanitize=address,undefined
address-san: $(TARGET)

library: $(SOURCES)
	$(CC) $(CFLAGS) $(LIBFLAGS) $(SOURCES) -c
	ar rcs src/lib$(LIB_NAME).a *.o

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f $(SHARED_TARGET)
	-rm -f src/*.a
	-rm -f src/gmon.out
	-rm -rf src/output
	-rm -f src/*.gcda
	-rm -f src/*.gcov
	-rm -f src/*.gcno
	-rm -f src/*.info
	-rm -f src/run_result.txt

%.o: %.c
	  $(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ $< -flto
