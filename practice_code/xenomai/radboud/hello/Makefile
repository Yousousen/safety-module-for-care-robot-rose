SKIN=alchemy
MAIN_SRC=hello
TARGET=hello
CC=gcc

LM=-lm

CFLAGS := $(shell xeno-config --posix --skin=$(SKIN) --cflags)
LDFLAGS := $(LM) $(shell xeno-config --posix --skin=$(SKIN) --ldflags)
#CC := $(shell xeno-config --cc)

$(TARGET): $(MAIN_SRC).c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
