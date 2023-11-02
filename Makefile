SHELL 	= /bin/sh
CC 		= clang-15

INCLUDES     = -I src/utils -I src/hashing -I src/list_data_structures -I src/data_structures -I src/sorting
FLAGS        = -std=c11 -fsanitize=cfi -fvisibility=hidden -D_DEFAULT_SOURCE
CFLAGS       = -pedantic-errors -Wall -Wextra -Werror -Wthread-safety -Wno-strict-prototypes
DEBUGFLAGS   = -O0 -Wno-unused -Wno-unused-parameter -fno-omit-frame-pointer -fno-sanitize-recover=all -g -D _DEBUG
RELEASEFLAGS = -O3 -fsanitize=safe-stack -D NDEBUG
LINKFLAGS 	 = -lcunit -flto -lm
WRAPFLAGS 	 = -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc
SHAREDFLAGS	 = -std=c11 -O3 -D NDEBUG -fPIC -shared -D_DEFAULT_SOURCE

TARGET			= src/test_runner
SHARED_TARGET	= algo.so
SOURCES			= $(wildcard src/*.c src/*/*.c src/*/include/*.c)
OBJECTS			= $(SOURCES:.c=.o)
DEPS			= $(OBJECTS:.o=.d)

all: $(TARGET)
all: CFLAGS += -fsanitize=safe-stack,undefined -fno-sanitize=vla-bound

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(INCLUDES) $(CFLAGS) $(DEBUGFLAGS) $(OBJECTS) $(LINKFLAGS) $(WRAPFLAGS) -o $(TARGET)
	ASAN_OPTIONS=detect_leaks=1 ./$(TARGET)

build-only: $(SOURCES)
	$(CC) $(FLAGS) $(INCLUDES) $(CFLAGS) $(RELEASEFLAGS) -o $(TARGET) $(SOURCES) $(LINKFLAGS)

release: $(SOURCES)
	$(CC) $(FLAGS) $(INCLUDES) $(CFLAGS) $(RELEASEFLAGS) -o $(TARGET) $(SOURCES) $(LINKFLAGS)
	./$(TARGET)

-include $(DEPS)

%.o: %.c
	$(CC) $(FLAGS) $(INCLUDES) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ $< -flto

%.d: %.c
	@$(CC) $(FLAGS) $(INCLUDES) $(CFLAGS) $(DEBUGFLAGS) $< -MM -MT $(@:.d=.o) >$@ -flto

profile: CFLAGS += -pg
profile: $(TARGET)

test-cases: FLAGS += -DTEST_CASES -DINSTRUMENT_SORT
test-cases: release

code-coverage: CFLAGS += -fprofile-arcs -ftest-coverage
code-coverage: $(TARGET)

memory-san: CFLAGS += -fsanitize=memory,undefined -fsanitize-memory-track-origins -fno-sanitize=vla-bound
memory-san: $(TARGET)

thread-san: CFLAGS += -fsanitize=thread,undefined -fno-sanitize=vla-bound
thread-san: $(TARGET)

address-san: CFLAGS += -fsanitize=address,undefined -fno-sanitize=vla-bound
address-san: $(TARGET)

shared: $(SOURCES)
	$(CC) $(INCLUDES) $(CFLAGS) $(SHAREDFLAGS) -o $(SHARED_TARGET) $(SOURCES) $(LINKFLAGS) $(WRAPFLAGS)

ctags:
	ctags -R .

comp-db:
	compiledb -- make -B build-only
	./fix_compile_commands.sh

lint:
	cpplint src/*.[ch] src/*/*.[ch]

format:
	clang-format-15 -i src/*.[ch] src/*/*.[ch]

tidy:
	clang-tidy-15 src/*.[ch] src/*/*.[ch]

clean:
	-rm -f $(OBJECTS)
	-rm -f $(DEPS)
	-rm -f $(TARGET)
	-rm -f $(SHARED_TARGET)
	-rm -f gmon.out
	-rm -rf code_coverage
	-rm -f src/*.gcda
	-rm -f src/*/*.gcda
	-rm -f src/*.gcov
	-rm -f src/*/*.gcov
	-rm -f src/*.gcno
	-rm -f src/*/*.gcno
	-rm -f src/*.info
	-rm -f src/run_result.txt
