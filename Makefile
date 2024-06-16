CC=gcc
CFLAGS=-I ./src -Wall

TARGET=./target
OBJ_DIR=$(TARGET)/obj
EXE_DIR=$(TARGET)/exe
SRC=./src

SRCS = $(shell find $(SRC) -name '*.c')
OBJ = $(SRCS:%.c=$(OBJ_DIR)/%.o)

$(EXE_DIR)/uuc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf $(EXE_DIR)/*
	rm -rf $(OBJ_DIR)/* 

