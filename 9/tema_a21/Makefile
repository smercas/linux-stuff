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

SRC = src
SOURCES = $(shell find $(SRC) -name "*.c")

OBJ = obj
OBJECTS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

BIN = bin
MAIN = $(BIN)/main

TESTFRAMEWORK = -lcriterion
TESTUSEDLIBRARIES = $(USEDLIBRARIES) $(TESTFRAMEWORK)

TEST = test

TESTSRC = $(TEST)/src
TESTSOURCES = $(shell find $(TESTSRC) -name "*.c")

TESTBIN = $(TEST)/bin
TESTBINARIES = $(patsubst $(TESTSRC)/%.c, $(TESTBIN)/%, $(TESTSOURCES))

all: $(MAIN)

release: CFLAGS = -Wall -O3 -DNDEBUG
release: clean
release: $(MAIN)

$(MAIN): $(OBJECTS) $(BIN)
	mkdir -p $(dir $@) ; $(CC) $(CFLAGS) $(LFLAGS) $(WLFLAGS) $(OBJECTS) -o $@ $(USEDLIBRARIES)

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h
	mkdir -p $(dir $@) ; $(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@ $(USEDLIBRARIES)

$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p $(dir $@) ; $(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@ $(USEDLIBRARIES)

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
	