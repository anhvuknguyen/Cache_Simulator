# Makefile for Cache_Simulator
# 
# Usage:
#   make           - Compile the entire project
#   make clean     - Remove all compiled files
#   make rebuild   - Clean and rebuild from scratch

# Compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
SRC_DIR = src
INC_DIR = include
OUT_DIR = bin
TST_DIR = testingScripts
OBJ_DIR = obj

# Executable name
EXECUTABLE = cacheSim

# Source files (all .cpp files)
SOURCES = 	$(TST_DIR)/test_CacheSetToString.cpp \
			$(SRC_DIR)/Cache_line.cpp \
			$(SRC_DIR)/Cache_set.cpp

# Object files (automatically generated from source files)
OBJECTS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SOURCES)))

# Header files (for dependency tracking)
HEADERS = 	$(INC_DIR)/Cache_line.h \
			$(INC_DIR)/Cache_set.h

# Default target - builds the executable
all: $(EXECUTABLE)

# Create required directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Link object files into executable
$(EXECUTABLE): $(OBJECTS) | $(OUT_DIR)
	@echo "Linking..."
	$(CXX) -o $(OUT_DIR)/$@ $^
	@echo "Build complete! Run with: ./$(OUT_DIR)/$(EXECUTABLE)"

# Compile .cpp files into .o object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/%.o: $(TST_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Clean up compiled files
clean:
	@echo "Cleaning build files..."
	rm -rf $(OBJ_DIR)
	rm -f $(OUT_DIR)/$(EXECUTABLE)
	@echo "Clean complete!"

# Rebuild from scratch
rebuild: clean all

# Help target - show available commands
help:
	@echo "Available targets:"
	@echo "  make          - Build the project (default)"
	@echo "  make clean    - Remove compiled files"
	@echo "  make rebuild  - Clean and rebuild"
	@echo "  make help     - Show this help message"

# Phony targets (not real files)
.PHONY: all clean rebuild help