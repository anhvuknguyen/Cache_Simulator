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

# Executable name
EXECUTABLE = cacheSim

# Source files (all .cpp files)
SOURCES = 	$(SRC_DIR)/main.cpp \
			$(SRC_DIR)/Cache_line.cpp\
			$(SRC_DIR)/Cache_set.cpp

# Object files (automatically generated from source files)
OBJECTS = $(SOURCES:.cpp=.o)

# Header files (for dependency tracking)
HEADERS = 	$(INC_DIR)/Cache_line.h \
			$(INC_DIR)/Cache_set.h

# Default target - builds the executable
all: $(EXECUTABLE)

# Link object files into executable
$(EXECUTABLE): $(OBJECTS)
	@echo "Linking..."
	$(CXX) -o $(OUT_DIR)/$@ $^
	@echo "Build complete! Run with: ./$(OUT_DIR)/$(EXECUTABLE)"

# Compile .cpp files into .o object files
%.o: %.cpp $(HEADERS)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Clean up compiled files
clean:
	@echo "Cleaning build files..."
	rm -f $(OBJECTS) $(OUT_DIR)/$(EXECUTABLE)
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