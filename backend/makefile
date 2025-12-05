# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

# Target executable
TARGET = test_btree

# Source files with correct paths
SOURCES = \
    src/data_structures/btree.cpp \
    src/models/vital_record.cpp \
    tests/test_btree.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link all object files
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile .cpp → .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean run
