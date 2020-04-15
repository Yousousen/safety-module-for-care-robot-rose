CXX = g++
CXXFLAGS = -g -std=c++11 -I$(RUNTIME) -I$(SRC) -lrt
CPPFLAGS = -I$(SRC) -I$(RUNTIME)
LDFLAGS = -g -lpthread
RM = rm -rf
TARGET = sm
VERSION = 2.9.0
CURRENT := $(shell dzn query | grep '*' | sed 's,\* ,,')

ifneq ($(VERSION),$(CURRENT))
$(info current version: $(CURRENT) is not equal to selected version: $(VERSION))
endif

SRC = ./src
RUNTIME = ./lib

SRCS = $(wildcard $(SRC)/*.cc)
SRCS += $(wildcard $(RUNTIME)/*.cc)

OBJS = $(subst .cc,.o,$(SRCS))

all:
	make $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

runtime: | $(RUNTIME)/dzn
	for f in $(shell dzn ls -R /share/runtime/c++ | sed 's,/c++/,,' | tail -n +3); do \
	dzn cat --version=$(VERSION) /share/runtime/c++/$$f > $(RUNTIME)/$$f; \
	done
	touch $@

$(RUNTIME)/dzn:
	mkdir -p $@

generate: $(wildcard *.dzn)
	for f in $^; do dzn -v code --version=$(VERSION) -l c++ -o $(SRC) $$f; done
	touch $@

clean:
	$(RM) $(OBJS) $(TARGET)

clean_generated: 
	$(RM) `grep -h dzn $(SRC)/*.dzn.d | sed -e 's,:.*,,' -e 's,%,.,g'`
	$(RM) $(RUNTIME)
	$(RM) runtime generate

distclean: clean clean_generated
	$(RM) *~

