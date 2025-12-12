# Compiler settings
CXX := g++
CXXFLAGS := -std=c++11 -Wall -g
LDFLAGS := -pthread

# Directories
SRC_DIR := src
DATA_STRUCT_DIR := $(SRC_DIR)/data_structures
MODELS_DIR := $(SRC_DIR)/models
TESTS_DIR := tests
INCLUDE_DIR := include

# Include paths
INCLUDES := -I$(SRC_DIR) -I$(DATA_STRUCT_DIR) -I$(MODELS_DIR) -I$(INCLUDE_DIR)

# Targets
TARGET_BTREE := test_btree
TARGET_HASHTABLE := test_hashtable
TARGET_PRIORITY_QUEUE := test_priority_queue
TARGET_DRUG_GRAPH := test_drug_graph
TARGET_SERVER := server

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

# Source files for Priority Queue
SOURCES_PRIORITY_QUEUE := \
	$(DATA_STRUCT_DIR)/priority_queue.cpp \
	$(MODELS_DIR)/alert.cpp \
	$(TESTS_DIR)/test_priority_queue.cpp

# Source files for Drug Graph
SOURCES_DRUG_GRAPH := \
	$(DATA_STRUCT_DIR)/drug_graph.cpp \
	$(TESTS_DIR)/test_drug_graph.cpp	

# Source files for Server
SOURCES_SERVER := \
	$(SRC_DIR)/server.cpp \
	$(DATA_STRUCT_DIR)/btree.cpp \
	$(DATA_STRUCT_DIR)/priority_queue.cpp \
	$(MODELS_DIR)/vital_record.cpp \
	$(MODELS_DIR)/patient.cpp \
	$(MODELS_DIR)/medication.cpp \
	$(MODELS_DIR)/alert.cpp \
	$(DATA_STRUCT_DIR)/drug_graph.cpp

# Object files
OBJECTS_BTREE := $(SOURCES_BTREE:.cpp=.o)
OBJECTS_HASHTABLE := $(SOURCES_HASHTABLE:.cpp=.o)
OBJECTS_PRIORITY_QUEUE := $(SOURCES_PRIORITY_QUEUE:.cpp=.o)
OBJECTDS_DRUG_GRAPH := $(SOURCES_DRUG_GRAPH:.cpp=.o)
OBJECTS_SERVER := $(SOURCES_SERVER:.cpp=.o)

# Default target
.PHONY: all
all: $(TARGET_BTREE) $(TARGET_HASHTABLE) $(TARGET_PRIORITY_QUEUE) $(TARGET_SERVER) $(TARGET_DRUG_GRAPH)

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

# Build Drug Graph test
$(TARGET_DRUG_GRAPH): $(OBJECTDS_DRUG_GRAPH)
	$(CXX) $(LDFLAGS) -o $@ $^
	@echo "✅ Drug Graph test compiled successfully!"

# Build Server
$(TARGET_SERVER): $(OBJECTS_SERVER)
	$(CXX) $(LDFLAGS) -o $@ $^
	@echo "✅ Server compiled successfully!"

# Build only specific targets
.PHONY: btree hashtable priority_queue server drug_graph
btree: $(TARGET_BTREE)
hashtable: $(TARGET_HASHTABLE)
priority_queue: $(TARGET_PRIORITY_QUEUE)
server: $(TARGET_SERVER)

# Compile .cpp → .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run tests
.PHONY: run-btree run-hashtable run-priority-queue run-drug-graph run-server
run-btree: $(TARGET_BTREE)
	@echo "Running B-tree tests..."
	./$(TARGET_BTREE)

run-hashtable: $(TARGET_HASHTABLE)
	@echo "Running Hash Table tests..."
	./$(TARGET_HASHTABLE)

run-priority-queue: $(TARGET_PRIORITY_QUEUE)
	@echo "Running Priority Queue tests..."
	./$(TARGET_PRIORITY_QUEUE)

run-drug-graph: $(TARGET_DRUG_GRAPH)
	@echo "Running Drug Graph tests..."
	./$(TARGET_DRUG_GRAPH)

run-server: $(TARGET_SERVER)
	@echo "Starting server..."
	./$(TARGET_SERVER)

# Run all tests (not server)
.PHONY: run
run: run-btree run-hashtable run-priority-queue run-drug-graph

# Clean
.PHONY: clean
clean:
	rm -f $(OBJECTS_BTREE) $(OBJECTS_HASHTABLE) $(OBJECTS_PRIORITY_QUEUE) $(OBJECTS_SERVER) $(OBJECTDS_DRUG_GRAPH)
	rm -f $(TARGET_BTREE) $(TARGET_HASHTABLE) $(TARGET_PRIORITY_QUEUE) $(TARGET_SERVER) $(TARGET_DRUG_GRAPH)
	rm -f *.bin
	@echo "🧹 Cleaned all build files"

# Help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  make all              - Build everything"
	@echo "  make server           - Build server only"
	@echo "  make run-server       - Build and run server"
	@echo "  make btree            - Build B-tree test"
	@echo "  make hashtable        - Build Hash Table test"
	@echo "  make priority_queue   - Build Priority Queue test"
	@echo "  make run              - Run all tests"
	@echo "  make clean            - Clean all build files"
	@echo "  make drug_graph       - Build Drug Graph test"
	@echo "  make run-drug-graph   - Run Drug Graph test"
	