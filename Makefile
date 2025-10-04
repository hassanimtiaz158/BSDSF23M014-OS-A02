# Makefile for lsv1.0.0 project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Files
SRC = $(SRC_DIR)/lsv1.0.0.c
OBJ = $(OBJ_DIR)/lsv1.0.0.o
TARGET = $(BIN_DIR)/ls

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source to object file
$(OBJ): $(SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure bin/ and obj/ exist
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

# Run the program after building
run: all
	./$(TARGET)
