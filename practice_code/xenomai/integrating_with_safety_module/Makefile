SKIN=native
MAIN_SRC=xenomai_sm
TARGET=xenomai_sm
CC=g++

LM=-lm

CFLAGS := $(shell xeno-config --skin=$(SKIN) --cflags)
LDFLAGS := $(LM) $(shell xeno-config --skin=$(SKIN) --ldflags)
#CC := $(shell xeno-config --cc)

$(TARGET): $(MAIN_SRC).cpp
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
