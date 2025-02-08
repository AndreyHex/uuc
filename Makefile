CC=gcc
CC_DEBUG=gcc -DUUC_LOG_DEBUG
CC_TRACE=gcc -DUUC_LOG_TRACE -DUUC_LOG_DEBUG
CFLAGS=-I ./src/include -Wall

TARGET=./target
OBJ_DIR=$(TARGET)/obj
BIN_DIR=$(TARGET)/bin
TEST_OBJ_DIR=$(TARGET)/test/obj
TEST_BIN_DIR=$(TARGET)/test/bin
SRC=./src

SRCS = $(shell find $(SRC) -name '*.c' -not -path '**/test/*')
TEST_SRCS = $(shell find $(SRC) -name '*.c' -not -name 'main.c')
OBJ = $(SRCS:%.c=$(OBJ_DIR)/%.o)
TEST_OBJ = $(TEST_SRCS:%.c=$(TEST_OBJ_DIR)/debug/%.o)
TEST_TRACE_OBJ = $(TEST_SRCS:%.c=$(TEST_OBJ_DIR)/trace/%.o)

$(BIN_DIR)/uuc: $(OBJ)
	mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(CFLAGS)

$(TEST_BIN_DIR)/debug: $(TEST_OBJ)
	mkdir -p $(dir $@)
	$(CC_DEBUG) -o $@ $^ $(CFLAGS)

$(TEST_BIN_DIR)/trace: $(TEST_TRACE_OBJ)
	mkdir -p $(dir $@)
	$(CC_TRACE) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

$(TEST_OBJ_DIR)/debug/%.o: %.c
	mkdir -p $(dir $@)
	$(CC_DEBUG) -c $< -o $@ $(CFLAGS)

$(TEST_OBJ_DIR)/trace/%.o: %.c
	mkdir -p $(dir $@)
	$(CC_TRACE) -c $< -o $@ $(CFLAGS)

build: $(BIN_DIR)/uuc

run: $(BIN_DIR)/uuc
	$(BIN_DIR)/uuc

run-trace: CC := $(CC_TRACE)
run-trace: clean $(BIN_DIR)/uuc
	$(BIN_DIR)/uuc

test: clean-test $(TEST_BIN_DIR)/debug
	$(TEST_BIN_DIR)/debug

test-trace: clean-test-trace $(TEST_BIN_DIR)/trace
	$(TEST_BIN_DIR)/trace

.PHONY: clean

clean: clean-test clean-test-trace clean-target

clean-test-trace:
	echo "Clearing target/test-trace directory"
	rm -rf $(TEST_OBJ_DIR)/trace
	rm -rf $(TEST_BIN_DIR)/trace

clean-test:
	echo "Clearing target/test directory"
	rm -rf $(TEST_OBJ_DIR)/debug
	rm -rf $(TEST_BIN_DIR)/debug

clean-target:
	echo "Clearing target directory"
	rm -rf $(OBJ_DIR)/*
	rm -rf $(BIN_DIR)/*
