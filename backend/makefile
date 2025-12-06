# Compiler settings
CXX := g++
CXXFLAGS := -std=c++11 -Wall -g
LDFLAGS :=

# Directories
SRC_DIR := src
DATA_STRUCT_DIR := $(SRC_DIR)/data_structures
MODELS_DIR := $(SRC_DIR)/models
TESTS_DIR := tests

# Include paths
INCLUDES := -I$(SRC_DIR) -I$(DATA_STRUCT_DIR) -I$(MODELS_DIR)

# Targets
TARGET_BTREE := test_btree
TARGET_HASHTABLE := test_hashtable
TARGET_PRIORITY_QUEUE := test_priority_queue

# Source files for B-tree
SOURCES_BTREE := \
	$(DATA_STRUCT_DIR)/btree.cpp \
	$(MODELS_DIR)/vital_record.cpp \
	$(TESTS_DIR)/test_btree.cpp

# Source files for Hash Table
SOURCES_HASHTABLE := \
	$(DATA_STRUCT_DIR)/hash_table.cpp \
	$(MODELS_DIR)/patient.cpp \
	$(MODELS_DIR)/medication.cpp \
	$(TESTS_DIR)/test_hashtable.cpp

#source files for Priority Queue (if needed in future)
SOURCES_PRIORITY_QUEUE := \
	$(DATA_STRUCT_DIR)/priority_queue.cpp \
	$(MODELS_DIR)/alert.cpp \
	$(TESTS_DIR)/test_priority_queue.cpp	

# Object files
OBJECTS_BTREE := $(SOURCES_BTREE:.cpp=.o)
OBJECTS_HASHTABLE := $(SOURCES_HASHTABLE:.cpp=.o)
OBJECTS_PRIORITY_QUEUE := $(SOURCES_PRIORITY_QUEUE:.cpp=.o)

# Default target
.PHONY: all
all: $(TARGET_BTREE) $(TARGET_HASHTABLE) $(TARGET_PRIORITY_QUEUE)

# Build B-tree test
$(TARGET_BTREE): $(OBJECTS_BTREE)
	$(CXX) $(LDFLAGS) -o $@ $^
	@echo "✅ B-tree test compiled successfully!"

# Build Hash Table test
$(TARGET_HASHTABLE): $(OBJECTS_HASHTABLE)
	$(CXX) $(LDFLAGS) -o $@ $^
	@echo "✅ Hash Table test compiled successfully!"
# Build Priority Queue test
$(TARGET_PRIORITY_QUEUE): $(OBJECTS_PRIORITY_QUEUE)
	$(CXX) $(LDFLAGS) -o $@ $^
	@echo "✅ Priority Queue test compiled successfully!"

# Build only B-tree
.PHONY: btree
btree: $(TARGET_BTREE)

# Build only Hash Table
.PHONY: hashtable
hashtable: $(TARGET_HASHTABLE)

# Build only Priority Queue
.PHONY: priority_queue
priority_queue: $(TARGET_PRIORITY_QUEUE)

# Compile .cpp → .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run B-tree test
.PHONY: run-btree
run-btree: $(TARGET_BTREE)
	@echo "Running B-tree tests..."
	./$(TARGET_BTREE)

# Run Hash Table test
.PHONY: run-hashtable
run-hashtable: $(TARGET_HASHTABLE)
	@echo "Running Hash Table tests..."
	./$(TARGET_HASHTABLE)

# Run Priority Queue test
.PHONY: run-priority-queue
run-priority-queue: $(TARGET_PRIORITY_QUEUE)
	@echo "Running Priority Queue tests..."
	./$(TARGET_PRIORITY_QUEUE)

# Run both tests
.PHONY: run
run: run-btree run-hashtable run-priority-queue
# Clean
.PHONY: clean
clean:
	rm -f $(OBJECTS_BTREE) $(OBJECTS_HASHTABLE) $(OBJECTS_PRIORITY_QUEUE)
	rm -f $(TARGET_BTREE) $(TARGET_HASHTABLE) $(TARGET_PRIORITY_QUEUE)
	rm -f *.bin
	@echo "🧹 Cleaned all build files"

# Clean only object files
.PHONY: clean-obj
clean-obj:
	rm -f $(OBJECTS_BTREE) $(OBJECTS_HASHTABLE)
	@echo "🧹 Cleaned object files"

# Help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  make all"
	@echo "  make btree"
	@echo "  make hashtable"
	@echo "  make run"
	@echo "  make run-btree"
	@echo "  make run-hashtable"
	@echo "  make clean"
	@echo "  make clean-obj"
	@echo "  make help"
	@echo "  make priority_queue"
	@echo "  make run-priority-queue"