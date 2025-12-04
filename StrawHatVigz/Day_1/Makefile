CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall

# Define targets
TARGET1 = day1_p1
SRC1 = day1_p1.cc

TARGET2 = day1_p2
SRC2 = day1_p2.cc

# 'all' builds both executables
all: $(TARGET1) $(TARGET2)

# Rule for Part 1
$(TARGET1): $(SRC1)
	$(CXX) $(CXXFLAGS) -o $(TARGET1) $(SRC1)

# Rule for Part 2
$(TARGET2): $(SRC2)
	$(CXX) $(CXXFLAGS) -o $(TARGET2) $(SRC2)

clean:
	rm -f $(TARGET1) $(TARGET2)

# Helpers to run specifically
run_p1: $(TARGET1)
	./$(TARGET1) input2.txt

run_p2: $(TARGET2)
	./$(TARGET2) input2.txt

.PHONY: all clean run_p1 run_p2