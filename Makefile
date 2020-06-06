SHELL 	= /bin/sh
CC 		= clang

FLAGS        = -std=c11 -fsanitize=cfi -fvisibility=hidden
CFLAGS       = -pedantic-errors -Wall -Wextra -Werror -Wthread-safety
DEBUGFLAGS   = -O0 -Wno-unused -Wno-unused-parameter -fno-omit-frame-pointer -fno-sanitize-recover=all -g -D _DEBUG
RELEASEFLAGS = -O3 -fsanitize=safe-stack -D NDEBUG
LINKFLAGS 	 = -lcunit -flto -lm -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc

TARGET			= src/test_runner
LIB_NAME	 	?=utils
SOURCES			= $(wildcard */*.c */*/*.c)
OBJECTS			= $(SOURCES:.c=.o)
DEPS			= $(OBJECTS:.o=.d)

all: $(TARGET)
all: CFLAGS += -fsanitize=safe-stack,undefined

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) $(OBJECTS) $(LINKFLAGS) -o $(TARGET) 
	ASAN_OPTIONS=detect_leaks=1 ./$(TARGET)

release: $(SOURCES)
	$(CC) $(FLAGS) $(CFLAGS) $(RELEASEFLAGS) -o $(TARGET) $(SOURCES) $(LINKFLAGS)
	./$(TARGET)

-include $(DEPS)

%.o: %.c
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ $< -flto

%.d: %.c
	@$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) $< -MM -MT $(@:.d=.o) >$@ -flto

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

comp-db:
	bear make -B release

lint:
	cpplint src/*.c src/*.h

format:
	clang-format -i src/*.c
	clang-format -i src/*.h

static-ana: comp-db
static-ana:
	clang-tidy src/*.[ch] 

clean:
	-rm -f $(OBJECTS)
	-rm -f $(DEPS)
	-rm -f $(TARGET)
	-rm -f $(SHARED_TARGET)
	-rm -f src/*.a
	-rm -f src/gmon.out
	-rm -rf src/output
	-rm -f src/*.gcda
	-rm -f src/include/*.gcda
	-rm -f src/*.gcov
	-rm -f src/*.gcno
	-rm -f src/include/*.gcno
	-rm -f src/*.info
	-rm -f src/run_result.txt

