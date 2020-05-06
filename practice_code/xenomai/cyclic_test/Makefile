SKIN=alchemy
MAIN_SRC=cyclic_test
TARGET=cyclic_test
CC=gcc

LM=-lm

CFLAGS := $(shell xeno-config --skin=$(SKIN) --cflags)
LDFLAGS := $(LM) $(shell xeno-config --skin=$(SKIN) --ldflags)
#CC := $(shell xeno-config --cc)

$(TARGET): $(MAIN_SRC).c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
