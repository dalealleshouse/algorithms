SHELL 	= /bin/sh
CC 		= clang-6.0

FLAGS        = -std=c11 -fsanitize=cfi -fvisibility=hidden 
CFLAGS       = -pedantic-errors -Wall -Wextra -Werror -Wthread-safety
DEBUGFLAGS   = -O0 -fno-omit-frame-pointer -fno-sanitize-recover=all -g -D _DEBUG
RELEASEFLAGS = -O3 -fsanitize=safe-stack -D NDEBUG
LINKFLAGS 	 = -lcunit -flto

TARGET	= test_runner
SOURCES	= $(shell echo /src/*.c)
OBJECTS	= $(SOURCES:.c=.o)

all: $(TARGET)
all: CFLAGS += -fsanitize=safe-stack,undefined

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -o $(TARGET) $(OBJECTS) $(LINKFLAGS)
	ASAN_OPTIONS=detect_leaks=1 ./$(TARGET)


release: $(SOURCES)
	$(CC) $(FLAGS) $(CFLAGS) $(RELEASEFLAGS) -o $(TARGET) $(SOURCES) $(LINKFLAGS)
	./$(TARGET)

profile: CFLAGS += -pg
profile: $(TARGET)

memory-san: CFLAGS += -fsanitize=memory,undefined
memory-san: $(TARGET)

thread-san: CFLAGS += -fsanitize=thread,undefined
thread-san: $(TARGET)

address-san: CFLAGS += -fsanitize=address,undefined
address-san: $(TARGET)

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f gmon.out

%.o: %.c
	  $(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ $< -flto
