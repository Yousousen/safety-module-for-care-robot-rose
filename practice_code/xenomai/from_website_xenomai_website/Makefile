XENO_CONFIG := /usr/xenomai/bin/xeno-config
CFLAGS := $(shell $(XENO_CONFIG) --vxworks --cflags)
LDFLAGS := $(shell $(XENO_CONFIG) --vxworks --ldflags)
CC := $(shell $(XENO_CONFIG) --cc)

EXECUTABLE := vxapp

all: $(EXECUTABLE)

%: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)
