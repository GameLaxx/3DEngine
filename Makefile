# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lSDL2 -lm
SRC_DIR = src
BUILD_DIR = obj
TEST_DIR = tests/unitary

# Source files
SRC_FILES = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Default target
all: main

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compiling object files (excluding main.c)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

# Main target
main: $(OBJ_FILES) $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) $(OBJ_FILES) $(SRC_DIR)/main.c -o engine $(LDFLAGS)
	./engine

# Test target
test: $(OBJ_FILES) $(TEST_DIR)/test_coordinates.c
	$(CC) $(CFLAGS) $(TEST_DIR)/test_coordinates.c $(OBJ_FILES) -o test $(LDFLAGS)
	./test

# Clean target to remove generated files
clean:
	rm -rf $(BUILD_DIR) engine test

# Phony targets
.PHONY: all clean test main
