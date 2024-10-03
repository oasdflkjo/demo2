# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
INCLUDES = -Iinclude
LIBS = -lopengl32 -lgdi32

# Directories
SRC_DIR = src
BUILD_DIR = build
EXECUTABLE = game

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Determine the number of CPU cores for parallel compilation
NPROCS = $(shell nproc)
MAKEFLAGS += -j$(NPROCS)

# Default target
all: $(EXECUTABLE)

# Link object files to create executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE)

# Phony targets
.PHONY: all clean