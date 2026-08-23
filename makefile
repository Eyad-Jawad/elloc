CC = gcc
CFLAGS = -Wall -g
TARGET = app
SRCS = src/main.c src/elloc.h

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clear:
	rm -f $(TARGET)