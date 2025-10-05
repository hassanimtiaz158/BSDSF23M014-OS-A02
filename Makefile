# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source and target
SRC = $(SRC_DIR)/ls-v1.3.0.c
OBJ = $(OBJ_DIR)/ls-v1.3.0.o
TARGET = $(BIN_DIR)/ls-v1.3.0

# Default target
all: $(TARGET)

# Create obj directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile .c -> .o
$(OBJ): $(SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link .o -> executable
$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/ls-v1.2.0

.PHONY: all clean
