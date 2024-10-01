# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
INCLUDES = -Iinclude
LIBS = -lopengl32 -lgdi32

# Directories
SRC_DIR = src
BUILD_DIR = build
EXECUTABLE = game  # Changed from $(BUILD_DIR)/game to game

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

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
	rm -rf $(BUILD_DIR) $(EXECUTABLE)  # Updated to remove both build folder and executable

# Phony targets
.PHONY: all clean