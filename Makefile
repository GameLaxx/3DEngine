# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g -I$(SRC_DIR) -I$(RT_DIR) -I$(SOFT_DIR) -I$(SCENES_DIR)
LDFLAGS = -lSDL2 -lSDL2_ttf -lm
SRC_DIR = src
RT_DIR = $(SRC_DIR)/RenderTools
SOFT_DIR = $(SRC_DIR)/Software
SCENES_DIR = $(SRC_DIR)/Scenes
BUILD_DIR = obj
TEST_DIR = tests/unitary

# Source files
SRC_FILES = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c)) \
            $(wildcard $(RT_DIR)/*.c) \
            $(wildcard $(SOFT_DIR)/*.c) \
            $(wildcard $(SCENES_DIR)/*.c)
OBJ_FILES = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRC_FILES)))

# Default target
all: main

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compiling object files (excluding main.c)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)
$(BUILD_DIR)/%.o: $(RT_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)
$(BUILD_DIR)/%.o: $(SOFT_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)
$(BUILD_DIR)/%.o: $(SCENES_DIR)/%.c | $(BUILD_DIR)
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
