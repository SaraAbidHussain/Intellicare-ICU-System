#include "priority_queue.h"
#include <algorithm>
#include <iomanip>

PriorityQueue::PriorityQueue(const std::string& filePath) 
    : dataFilePath(filePath) {
    if (!dataFilePath.empty()) {
        loadFromDisk();
    }
}

PriorityQueue::~PriorityQueue() {
    if (!dataFilePath.empty()) {
        saveToDisk();
    }
}

// Swap two elements in heap
void PriorityQueue::swap(int i, int j) {
    Alert temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

// Move element up to maintain heap property
void PriorityQueue::heapifyUp(int index) {
    // If not root and parent is greater, swap
    while (index > 0 && heap[parent(index)] > heap[index]) {
        swap(index, parent(index));
        index = parent(index);
    }
}

// Move element down to maintain heap property
void PriorityQueue::heapifyDown(int index) {
    int minIndex = index;
    int left = leftChild(index);
    int right = rightChild(index);
    
    // Find smallest among node, left child, right child
    if (left < heap.size() && heap[left] < heap[minIndex]) {
        minIndex = left;
    }
    
    if (right < heap.size() && heap[right] < heap[minIndex]) {
        minIndex = right;
    }
    
    // If smallest is not current node, swap and continue
    if (minIndex != index) {
        swap(index, minIndex);
        heapifyDown(minIndex);
    }
}

// Insert new alert into priority queue
void PriorityQueue::insert(const Alert& alert) {
    heap.push_back(alert);
    heapifyUp(heap.size() - 1);
    
    std::cout << "[PQ] Inserted alert ID " << alert.alertID 
              << " (Priority: " << alert.getPriorityString() << ")" << std::endl;
}

// Extract and return highest priority alert (minimum)
Alert PriorityQueue::extractMin() {
    if (isEmpty()) {
        throw std::runtime_error("Priority queue is empty!");
    }
    
    // Root is the minimum element
    Alert minAlert = heap[0];
    
    // Move last element to root
    heap[0] = heap.back();
    heap.pop_back();
    
    // Restore heap property
    if (!isEmpty()) {
        heapifyDown(0);
    }
    
    std::cout << "[PQ] Extracted alert ID " << minAlert.alertID 
              << " (Priority: " << minAlert.getPriorityString() << ")" << std::endl;
    
    return minAlert;
}

// Peek at highest priority alert without removing
Alert PriorityQueue::peekMin() const {
    if (isEmpty()) {
        throw std::runtime_error("Priority queue is empty!");
    }
    return heap[0];
}

// Display all alerts in priority order
void PriorityQueue::display() const {
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║          PRIORITY QUEUE - ALL ALERTS              ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    
    if (isEmpty()) {
        std::cout << "\n  No alerts in queue.\n" << std::endl;
        return;
    }
    
    // Create copy and extract in order to display
    std::vector<Alert> sortedAlerts = heap;
    std::make_heap(sortedAlerts.begin(), sortedAlerts.end(), std::greater<Alert>());
    
    int count = 1;
    while (!sortedAlerts.empty()) {
        std::pop_heap(sortedAlerts.begin(), sortedAlerts.end(), std::greater<Alert>());
        Alert alert = sortedAlerts.back();
        sortedAlerts.pop_back();
        
        std::cout << "\n[" << count++ << "] ";
        alert.display();
    }
    
    std::cout << "\nTotal alerts: " << heap.size() << std::endl;
}

// Display heap structure (for debugging)
void PriorityQueue::displayTree() const {
    std::cout << "\n========== Heap Structure ==========" << std::endl;
    
    if (isEmpty()) {
        std::cout << "Empty heap" << std::endl;
        return;
    }
    
    int level = 0;
    int nodesInLevel = 1;
    int nodesPrinted = 0;
    
    for (int i = 0; i < heap.size(); i++) {
        if (nodesPrinted == 0) {
            std::cout << "Level " << level << ": ";
        }
        
        std::cout << "[" << heap[i].alertID << ":" << heap[i].priority << "] ";
        nodesPrinted++;
        
        if (nodesPrinted == nodesInLevel || i == heap.size() - 1) {
            std::cout << std::endl;
            level++;
            nodesInLevel *= 2;
            nodesPrinted = 0;
        }
    }
    
    std::cout << "====================================\n" << std::endl;
}

// Get all alerts of specific priority
std::vector<Alert> PriorityQueue::getAlertsByPriority(AlertPriority prio) const {
    std::vector<Alert> result;
    for (const auto& alert : heap) {
        if (alert.priority == prio) {
            result.push_back(alert);
        }
    }
    return result;
}

// Get all unacknowledged alerts
std::vector<Alert> PriorityQueue::getUnacknowledgedAlerts() const {
    std::vector<Alert> result;
    for (const auto& alert : heap) {
        if (!alert.acknowledged) {
            result.push_back(alert);
        }
    }
    return result;
}

// Clear all alerts
void PriorityQueue::clear() {
    heap.clear();
    std::cout << "[PQ] All alerts cleared" << std::endl;
}

// Save to disk
void PriorityQueue::saveToDisk() {
    if (dataFilePath.empty()) return;
    
    std::ofstream file(dataFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[PQ] Error: Cannot open file for writing: " 
                  << dataFilePath << std::endl;
        return;
    }
    
    // Write number of alerts
    int numAlerts = heap.size();
    file.write(reinterpret_cast<const char*>(&numAlerts), sizeof(numAlerts));
    
    // Write all alerts
    for (const auto& alert : heap) {
        alert.writeToDisk(file);
    }
    
    file.close();
    std::cout << "[PQ] Saved " << numAlerts << " alerts to " << dataFilePath << std::endl;
}

// Load from disk
void PriorityQueue::loadFromDisk() {
    if (dataFilePath.empty()) return;
    
    std::ifstream file(dataFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "[PQ] No existing data file found: " << dataFilePath << std::endl;
        return;
    }
    
    // Read number of alerts
    int numAlerts;
    file.read(reinterpret_cast<char*>(&numAlerts), sizeof(numAlerts));
    
    // Read all alerts
    heap.clear();
    for (int i = 0; i < numAlerts; i++) {
        Alert alert;
        alert.readFromDisk(file);
        heap.push_back(alert);
    }
    
    file.close();
    std::cout << "[PQ] Loaded " << numAlerts << " alerts from " << dataFilePath << std::endl;
}