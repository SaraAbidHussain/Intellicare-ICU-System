#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "../models/alert.h"

class PriorityQueue {
private:
    std::vector<Alert> heap;
    std::string dataFilePath;
    
    // Helper functions for heap operations
    int parent(int i) const { return (i - 1) / 2; }
    int leftChild(int i) const { return 2 * i + 1; }
    int rightChild(int i) const { return 2 * i + 2; }
    
    // Heap maintenance operations
    void heapifyUp(int index);
    void heapifyDown(int index);
    void swap(int i, int j);
    
public:
    // Constructor & Destructor
    PriorityQueue(const std::string& filePath = "");
    ~PriorityQueue();
    
    // Main operations
    void insert(const Alert& alert);
    Alert extractMin();
    Alert peekMin() const;
    
    // Utility
    bool isEmpty() const { return heap.empty(); }
    int size() const { return heap.size(); }
    void display() const;
    void displayTree() const;
    
    // Get alerts by priority
    std::vector<Alert> getAlertsByPriority(AlertPriority prio) const;
    std::vector<Alert> getUnacknowledgedAlerts() const;
    
    // Disk persistence
    void saveToDisk();
    void loadFromDisk();
    
    // Clear all alerts
    void clear();
};

#endif