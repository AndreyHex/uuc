CC=gcc
CFLAGS=-I ./src -Wall

TARGET=./target
OBJ_DIR=$(TARGET)/obj
EXE_DIR=$(TARGET)/exe
TEST_EXE_DIR=$(TARGET)/exe/test
SRC=./src

SRCS = $(shell find $(SRC) -name '*.c' -not -path '**/test/*')
TEST_SRCS = $(shell find $(SRC) -name '*.c' -not -name 'main.c')
OBJ = $(SRCS:%.c=$(OBJ_DIR)/%.o)
TEST_OBJ = $(TEST_SRCS:%.c=$(OBJ_DIR)/%.o)

$(EXE_DIR)/uuc: $(OBJ)
	mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(CFLAGS)

$(TEST_EXE_DIR)/uuc_test: $(TEST_OBJ)
	mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

run: $(EXE_DIR)/uuc
	$(EXE_DIR)/uuc

test: $(TEST_EXE_DIR)/uuc_test
	$(TEST_EXE_DIR)/uuc_test

.PHONY: clean

clean:
	rm -rf $(EXE_DIR)/*
	rm -rf $(OBJ_DIR)/*
