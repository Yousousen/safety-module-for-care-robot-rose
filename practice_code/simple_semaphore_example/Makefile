SKIN=posix
MAIN_SRC=semaphores
TARGET=semaphores
CC=g++

CFLAGS := -g 
LDFLAGS := -g -pthread

$(TARGET): $(MAIN_SRC).cpp
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
