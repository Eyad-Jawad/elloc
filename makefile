CC = gcc

CFLAGS = -Wall -Wextra -Werror -g
MEMORYFLAGS = -fsanitize=address -fno-omit-frame-pointer
LIBS = -lcriterion

TARGET = app
TESTTARGET = test_runner

SRCS = $(wildcard src/*.c)
TESTSRCS = $(wildcard tests/*.c) $(filter-out src/main.c, $(wildcard src/*.c))

.PHONY: all test clean


all: $(TARGET)

test: $(TESTTARGET)
	./$(TESTTARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(MEMORYFLAGS) $^ -o $@

$(TESTTARGET): $(TESTSRCS)
	$(CC) $(CFLAGS) $(MEMORYFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(TARGET) $(TESTTARGET)