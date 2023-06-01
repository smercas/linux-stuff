CC = gcc
WARNINGS = -Wall -Wextra
OPTIMIZATIONS = -O0
DEBUGINFO = -g3
CFLAGS = $(WARNINGS) $(DEBUGINFO) $(OPTIMIZATIONS)
USEDLIBRARIES = -lc -lc_utils

CUTILSPATH = /home/stefan/Documents/c_utils
# change this to where you've placed the c_utils shared library

IFLAGS = -I$(CUTILSPATH)/src
LFLAGS = -L$(CUTILSPATH)/lib
WLFLAGS = -Wl,-rpath=$(CUTILSPATH)/lib

NAMES = a b c

SRC = src
SOURCES = $(foreach name, $(NAMES), $(SRC)/$(name).c)

OBJ = obj
OBJECTS = $(foreach name, $(NAMES), $(OBJ)/$(name).o)

BIN = bin
MAINS = $(foreach name, $(NAMES), $(BIN)/$(name))

TESTFRAMEWORK = -lcriterion
TESTUSEDLIBRARIES = $(USEDLIBRARIES) $(TESTFRAMEWORK)

TEST = test

TESTSRC = $(TEST)/src
TESTSOURCES = $(shell find $(TESTSRC) -name "*.c")

TESTBIN = $(TEST)/bin
TESTBINARIES = $(patsubst $(TESTSRC)/%.c, $(TESTBIN)/%, $(TESTSOURCES))

.PRECIOUS: $(OBJECTS)

all: $(MAINS)

release: CFLAGS = -Wall -O3 -DNDEBUG
release: clean
release: $(MAINS)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@ $(USEDLIBRARIES)

$(BIN)/%: $(OBJ)/%.o | $(BIN)
	$(CC) $(CFLAGS) $(LFLAGS) $(WLFLAGS) $< -o $@ $(USEDLIBRARIES)

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

$(TESTBIN)/%: $(TESTSRC)/%.c $(MAIN)
	mkdir -p $(dir $@) ; $(CC) $(CFLAGS) $(IFLAGS) $(LFLAGS) $(WLFLAGS) $^ -o $@ $(TESTUSEDLIBRARIES)

$(TEST):
	mkdir $@

$(TESTBIN): $(TEST)
	mkdir $@

$(TESTSRC): $(TEST)
	mkdir $@

runtest: $(BIN) $(TESTBIN) $(TESTBINARIES)
	for test in $(TESTBINARIES) ; do ./$$test ; done

clean:
	$(RM) -r $(BIN)/* $(OBJ)/* $(TESTBIN)/*
