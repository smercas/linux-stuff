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

MASTER = master
SLAVE = slave

SRC = src
MASTERSRC = $(SRC)/$(MASTER).c
SLAVESRC = $(SRC)/$(SLAVE).c

OBJ = obj
MASTEROBJ = $(OBJ)/$(MASTER).o
SLAVEOBJ = $(OBJ)/$(SLAVE).o

BIN = bin
MASTERMAIN = $(BIN)/$(MASTER)
SLAVEMAIN = $(BIN)/$(SLAVE)

TESTFRAMEWORK = -lcriterion
TESTUSEDLIBRARIES = $(USEDLIBRARIES) $(TESTFRAMEWORK)

TEST = test

TESTSRC = $(TEST)/src
TESTSOURCES = $(shell find $(TESTSRC) -name "*.c")

TESTBIN = $(TEST)/bin
TESTBINARIES = $(patsubst $(TESTSRC)/%.c, $(TESTBIN)/%, $(TESTSOURCES))

all: $(MASTERMAIN) $(SLAVEMAIN)

release: CFLAGS = -Wall -O3 -DNDEBUG
release: clean
release: $(MASTERMAIN) $(SLAVEMAIN)

$(MASTERMAIN): $(MASTEROBJ) | $(BIN)
	$(CC) $(CFLAGS) $(LFLAGS) $(WLFLAGS) $< -o $@ $(USEDLIBRARIES)

$(SLAVEMAIN): $(SLAVEOBJ) | $(BIN)
	$(CC) $(CFLAGS) $(LFLAGS) $(WLFLAGS) $< -o $@ $(USEDLIBRARIES)

$(MASTEROBJ): $(MASTERSRC) | $(OBJ)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@ $(USEDLIBRARIES)

$(SLAVEOBJ): $(SLAVESRC) | $(OBJ)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@ $(USEDLIBRARIES)

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
