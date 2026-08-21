CC = gcc
CFLAGS = -Wall -g
TARGET = app
SRCS = main.c main.h

all: $(TARGET)

$(TARGET) : $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clear:
	rm -f $(TARGET)