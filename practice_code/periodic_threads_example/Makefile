SKIN=posix
MAIN_SRC=periodic-thread
TARGET=periodic-thread
CC=gcc

LM=-lm

CFLAGS := -g $(shell xeno-config --posix --skin=$(SKIN) --cflags)
LDFLAGS := -g $(LM) $(shell xeno-config --posix --skin=$(SKIN) --ldflags)
#CC := $(shell xeno-config --cc)

$(TARGET): $(MAIN_SRC).c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
