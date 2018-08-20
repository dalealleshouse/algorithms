SHELL 	= /bin/sh
CC 		= clang-6.0

FLAGS        = -std=c11 -fsanitize=cfi -fvisibility=hidden 
CFLAGS       = -pedantic-errors -Wall -Wextra -Werror -Wthread-safety
DEBUGFLAGS   = -O0 -Wno-unused -Wno-unused-parameter -fno-omit-frame-pointer -fno-sanitize-recover=all -g -D _DEBUG
RELEASEFLAGS = -O3 -fsanitize=safe-stack -D NDEBUG
SHAREDFLAGS	 = -std=c11 -O3 -D NDEBUG -fPIC -shared
LINKFLAGS 	 = -lcunit -flto -lm

TARGET			= src/test_runner
SHARED_TARGET	= src/algo.so
SOURCES			= $(shell echo src/*.c)
OBJECTS			= $(SOURCES:.c=.o)

all: $(TARGET)
all: CFLAGS += -fsanitize=safe-stack,undefined

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -o $(TARGET) $(OBJECTS) $(LINKFLAGS)
	ASAN_OPTIONS=detect_leaks=1 ./$(TARGET)

release: $(SOURCES)
	$(CC) $(FLAGS) $(CFLAGS) $(RELEASEFLAGS) -o $(TARGET) $(SOURCES) $(LINKFLAGS)
	./$(TARGET)

shared: $(SOURCES)
	$(CC) $(CFLAGS) $(SHAREDFLAGS) -o $(SHARED_TARGET) $(SOURCES) $(LINKFLAGS)

profile: CFLAGS += -pg
profile: $(TARGET)

code-coverage: CFLAGS += -coverage
code-coverage: $(TARGET)

memory-san: CFLAGS += -fsanitize=memory,undefined
memory-san: $(TARGET)

thread-san: CFLAGS += -fsanitize=thread,undefined
thread-san: $(TARGET)

address-san: CFLAGS += -fsanitize=address,undefined
address-san: $(TARGET)

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f $(SHARED_TARGET)
	-rm -f gmon.out

%.o: %.c
	  $(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ $< -flto
