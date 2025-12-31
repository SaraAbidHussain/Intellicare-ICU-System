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
SIM_DIR := $(SRC_DIR)/simulation
ANALYSIS_DIR := $(SRC_DIR)/analysis


# Include paths
INCLUDES := -I$(SRC_DIR) -I$(DATA_STRUCT_DIR) -I$(MODELS_DIR) -I$(INCLUDE_DIR) -I$(SIM_DIR) -I$(ANALYSIS_DIR)

# Targets
TARGET_BTREE := test_btree
TARGET_HASHTABLE := test_hashtable
TARGET_PRIORITY_QUEUE := test_priority_queue
TARGET_DRUG_GRAPH := test_drug_graph
TARGET_KDTREE := test_kdtree
TARGET_SLIDING_WINDOW := test_sliding_window
TARGET_CIRCULAR_BUFFER := test_circular_buffer

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

# Source files for KD-Tree
SOURCES_KDTREE := \
	$(DATA_STRUCT_DIR)/kdtree.cpp \
	$(MODELS_DIR)/vital_record.cpp \
	$(TESTS_DIR)/test_kdtree.cpp	
# Source files for Sliding Window
SOURCES_SLIDING_WINDOW := \
	$(DATA_STRUCT_DIR)/sliding_window.cpp \
	$(MODELS_DIR)/vital_record.cpp \
	$(TESTS_DIR)/test_sliding_window.cpp
# Source files for Circular Buffer
SOURCES_CIRCULAR_BUFFER := \
	$(MODELS_DIR)/vital_record.cpp \
	$(TESTS_DIR)/test_circular_buffer.cpp
# Source files for Server
SOURCES_SERVER := \
	$(SRC_DIR)/server.cpp \
	$(DATA_STRUCT_DIR)/btree.cpp \
	$(DATA_STRUCT_DIR)/priority_queue.cpp \
	$(DATA_STRUCT_DIR)/hash_table.cpp \
	$(DATA_STRUCT_DIR)/kdtree.cpp \
	$(DATA_STRUCT_DIR)/sliding_window.cpp \
	$(DATA_STRUCT_DIR)/circular_buffer.cpp \
	$(MODELS_DIR)/vital_record.cpp \
	$(MODELS_DIR)/patient.cpp \
	$(MODELS_DIR)/medication.cpp \
	$(MODELS_DIR)/alert.cpp \
	$(DATA_STRUCT_DIR)/drug_graph.cpp \
	$(SIM_DIR)/vital_simulator.cpp \
	$(ANALYSIS_DIR)/vital_analyzer.cpp

# Object files
OBJECTS_BTREE := $(SOURCES_BTREE:.cpp=.o)
OBJECTS_HASHTABLE := $(SOURCES_HASHTABLE:.cpp=.o)
OBJECTS_PRIORITY_QUEUE := $(SOURCES_PRIORITY_QUEUE:.cpp=.o)
OBJECTS_DRUG_GRAPH := $(SOURCES_DRUG_GRAPH:.cpp=.o)
OBJECTS_KDTREE := $(SOURCES_KDTREE:.cpp=.o)
OBJECTS_SLIDING_WINDOW := $(SOURCES_SLIDING_WINDOW:.cpp=.o)
OBJECTS_CIRCULAR_BUFFER := $(SOURCES_CIRCULAR_BUFFER:.cpp=.o)
OBJECTS_SERVER := $(SOURCES_SERVER:.cpp=.o)

# Default target
.PHONY: all
all: $(TARGET_BTREE) $(TARGET_HASHTABLE) $(TARGET_PRIORITY_QUEUE) $(TARGET_SERVER) $(TARGET_DRUG_GRAPH) $(TARGET_KDTREE) $(TARGET_SLIDING_WINDOW) $(TARGET_CIRCULAR_BUFFER) 

# Build B-tree test

$(TARGET_BTREE): $(OBJECTS_BTREE)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ B-tree test compiled successfully!"

# Build Hash Table test

$(TARGET_HASHTABLE): $(OBJECTS_HASHTABLE)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Hash Table test compiled successfully!"

# Build Priority Queue test

$(TARGET_PRIORITY_QUEUE): $(OBJECTS_PRIORITY_QUEUE)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Priority Queue test compiled successfully!"

# Build Drug Graph test

$(TARGET_DRUG_GRAPH): $(OBJECTS_DRUG_GRAPH)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Drug Graph test compiled successfully!"

# Build KD-Tree test

$(TARGET_KDTREE): $(OBJECTS_KDTREE)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ KD-Tree test compiled successfully!"

# Build Sliding Window test

$(TARGET_SLIDING_WINDOW): $(OBJECTS_SLIDING_WINDOW)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Sliding Window test compiled successfully!"

# Build Circular Buffer test

$(TARGET_CIRCULAR_BUFFER): $(OBJECTS_CIRCULAR_BUFFER)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Circular Buffer test compiled successfully!"

# Build Server

$(TARGET_SERVER): $(OBJECTS_SERVER)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "✅ Server compiled successfully!"

#create simulation and analysis directories
$(SIM_DIR):
	@mkdir -p $(SIM_DIR)

$(ANALYSIS_DIR):
	@mkdir -p $(ANALYSIS_DIR)

# Build only specific targets
.PHONY: btree hashtable priority_queue server drug_graph
btree: $(TARGET_BTREE)
hashtable: $(TARGET_HASHTABLE)
priority_queue: $(TARGET_PRIORITY_QUEUE)
drug_graph: $(TARGET_DRUG_GRAPH)
kdtree: $(TARGET_KDTREE)
sliding_window: $(TARGET_SLIDING_WINDOW)
circular_buffer: $(TARGET_CIRCULAR_BUFFER)
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

run-kdtree: $(TARGET_KDTREE)
	@echo "Running KD-Tree tests..."
	./$(TARGET_KDTREE)

run-sliding-window: $(TARGET_SLIDING_WINDOW)
	@echo "Running Sliding Window tests..."
	./$(TARGET_SLIDING_WINDOW)

run-circular-buffer: $(TARGET_CIRCULAR_BUFFER)
	@echo "Running Circular Buffer tests..."
	./$(TARGET_CIRCULAR_BUFFER)

run-server: $(TARGET_SERVER)
	@echo "Starting server..."
	./$(TARGET_SERVER)



# Run all tests (not server)
.PHONY: run
run: run-btree run-hashtable run-priority-queue run-drug-graph run-kdtree run-sliding-window run-circular-buffer

# Clean
.PHONY: clean
clean:
	rm -f $(OBJECTS_BTREE) $(OBJECTS_HASHTABLE) $(OBJECTS_PRIORITY_QUEUE) $(OBJECTS_SERVER) $(OBJECTS_DRUG_GRAPH) $(OBJECTS_KDTREE) $(OBJECTS_SLIDING_WINDOW) $(OBJECTS_CIRCULAR_BUFFER)
	rm -f $(TARGET_BTREE) $(TARGET_HASHTABLE) $(TARGET_PRIORITY_QUEUE) $(TARGET_SERVER) $(TARGET_DRUG_GRAPH) $(TARGET_KDTREE) $(TARGET_SLIDING_WINDOW) $(TARGET_CIRCULAR_BUFFER)
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
	@echo "  make kdtree           - Build KD-Tree test"
	@echo "  make run-kdtree       - Run KD-Tree test"
	@echo "  make sliding_window   - Build Sliding Window test"
	@echo "  make run-sliding-window - Run Sliding Window test"
	@echo "  make circular_buffer  - Build Circular Buffer test"
	@echo "  make run-circular-buffer - Run Circular Buffer test"