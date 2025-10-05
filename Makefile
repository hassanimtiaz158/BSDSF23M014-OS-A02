# Makefile for ls-v1.5.0 project

# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Target executable
TARGET  = $(BIN_DIR)/ls-v1.5.0

# Source and object files
SRC     = $(SRC_DIR)/ls-v1.5.0.c
OBJ     = $(OBJ_DIR)/ls-v1.5.0.o

# Default target
all: $(TARGET)

# Link object file to create executable
$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source file to object file
$(OBJ): $(SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

# Phony targets
.PHONY: all clean
