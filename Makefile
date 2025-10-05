# -----------------------
# Makefile for ls project
# -----------------------

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Source and target
SRC = $(SRC_DIR)/ls-v1.4.0.c
OBJ = $(OBJ_DIR)/ls-v1.4.0.o
TARGET = $(BIN_DIR)/ls-v1.4.0

# Default target
all: $(TARGET)

# Create bin and obj directories if they don't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile object file
$(OBJ): $(SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link final executable
$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

# Phony targets
.PHONY: all clean

