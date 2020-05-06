SKIN=alchemy
MAIN_SRC=cyclic_test
TARGET=cyclic_test

LM=-lm

CFLAGS := $(shell xeno-config --skin=alchemy --cflags)
LDFLAGS := $(LM) $(shell xeno-config --skin=alchemy --ldflags)
CC := $(shell xeno-config --cc)

$(TARGET): $(MAIN_SRC).c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
