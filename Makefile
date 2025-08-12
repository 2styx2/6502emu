CC = gcc
CFLAGS = -Wall -std=c99
SRC_DIR = src
BIN_DIR = bin
TEST_DIR = test
DEBUG_DIR = debug

SRCS = $(wildcard $(SRC_DIR)/*.c)

TARGET = main

.PHONY: clean all test debug

all: $(BIN_DIR)/$(TARGET)

debug-bulid: $(DEBUG_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(SRCS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(DEBUG_DIR)/$(TARGET): $(SRCS) | $(DEBUG_DIR)
	$(CC) $(CFLAGS) -g $^ -o $@

$(BIN_DIR):
	mkdir -p $@

$(DEBUG_DIR):
	mkdir -p $@

clean:
	rm -rf $(DEBUG_DIR) $(BIN_DIR)
