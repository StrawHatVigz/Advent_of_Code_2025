# Compiler to use
CXX = g++

# Compiler flags
# -Wall: Enable all warnings
# -std=c++11: Use C++11 standard
CXXFLAGS = -O3 -Wall -std=c++17

# Target executable name
TARGET = Day1

# Source files
SRC = Day1.cc

# Default target (build the executable)
all: $(TARGET)

# Rule to link the object file to create the executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Clean up build files
clean:
	rm -f $(TARGET)

# Phony targets help avoid conflicts with files named 'all' or 'clean'
.PHONY: all clean