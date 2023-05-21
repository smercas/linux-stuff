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

MASTER = master_wc-bin
SLAVE = slave_wc-bin

SRC = src
SOURCES = $(shell find $(SRC) -name "*.c")
MASTERSRC = $(SRC)/$(MASTER).c
SLAVESRC = $(SRC)/$(SLAVE).c

OBJ = obj
OBJECTS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
MASTEROBJ = $(OBJ)/$(MASTER).o
SLAVEOBJ = $(OBJ)/$(SLAVE).o

BIN = bin
MAIN = $(BIN)/main
MASTERMAIN = $(BIN)/master
SLAVEMAIN = $(BIN)/slave

TESTFRAMEWORK = -lcriterion
TESTUSEDLIBRARIES = $(USEDLIBRARIES) $(TESTFRAMEWORK)

TEST = test

TESTSRC = $(TEST)/src
TESTSOURCES = $(shell find $(TESTSRC) -name "*.c")

TESTBIN = $(TEST)/bin
TESTBINARIES = $(patsubst $(TESTSRC)/%.c, $(TESTBIN)/%, $(TESTSOURCES))

#all: $(MAIN)
all: $(MASTERMAIN) $(SLAVEMAIN)

release: CFLAGS = -Wall -O3 -DNDEBUG
release: clean
#release: $(MAIN)
release: $(MASTER) $(SLAVE)

$(MASTERMAIN): $(MASTEROBJ) $(BIN)
	mkdir -p $(dir $@) ; $(CC) $(CFLAGS) $(LFLAGS) $(WLFLAGS) $(MASTEROBJ) -o $@ $(USEDLIBRARIES)

$(SLAVEMAIN): $(SLAVEOBJ) $(BIN)
	mkdir -p $(dir $@) ; $(CC) $(CFLAGS) $(LFLAGS) $(WLFLAGS) $(SLAVEOBJ) -o $@ $(USEDLIBRARIES)

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
	mkdir -p $(dir $@) ; $(CC) $(CFLAGS) $^ -o $@ $(TESTUSEDLIBRARIES)

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
	