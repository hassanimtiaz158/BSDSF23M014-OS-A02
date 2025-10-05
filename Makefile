# Top-level Makefile
CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -g
TARGET  := ls-v1.2.0

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
BIN  := $(BIN_DIR)/$(TARGET)

.PHONY: all clean dirs run

all: dirs $(BIN)

# Link final binary from object files
$(BIN): $(OBJS)
	@echo "Linking -> $@"
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c -> obj/.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $< -> $@"
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure directories exist
dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Run the program (convenience)
run: all
	@echo "Running $(BIN)"
	$(BIN)

clean:
	@echo "Cleaning..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

# Print variables (debug)
print:
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"
	@echo "BIN  = $(BIN)"
