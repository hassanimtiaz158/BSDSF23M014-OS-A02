# ---------------- CONFIG -----------------
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TARGET = $(BIN_DIR)/ls-v1.6.0

# Source and object files
SRC = $(SRC_DIR)/ls-v1.6.0.c
OBJ = $(OBJ_DIR)/ls-v1.6.0.o

# ---------------- RULES -----------------
all: $(TARGET)

# Build object file from source
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build executable from object file
$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ) -o $@

# Create directories if they don't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/ls-v1.6.0

# Phony targets
.PHONY: all clean
