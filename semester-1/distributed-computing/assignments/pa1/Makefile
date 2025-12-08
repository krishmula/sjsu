CC = gcc
CFLAGS = -pthread -Wall
TARGET = server

.PHONY: all clean

all: $(TARGET)

$(TARGET): server.c
	$(CC) $(CFLAGS) -o $(TARGET) server.c

clean:
	rm -f $(TARGET)
