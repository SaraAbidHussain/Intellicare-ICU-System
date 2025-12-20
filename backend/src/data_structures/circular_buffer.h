#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "../models/vital_record.h"


// Generic circular buffer (ring buffer) for fixed-size sliding window
template<typename T>
class CircularBuffer {
private:
    std::vector<T> buffer;
    int capacity;
    int head;      // Points to next write position
    int tail;      // Points to oldest element
    int count;     // Number of elements currently stored
    bool overwrite;  // Whether to overwrite when full
    
public:
    // Constructor
    CircularBuffer(int cap = 100, bool allowOverwrite = true);
    
    // Add element (pushes out oldest if full and overwrite=true)
    void push(const T& item);
    
    // Remove oldest element
    T pop();
    
    // Peek at element without removing
    T peek() const;
    T peekAt(int index) const;  // 0 = oldest, size()-1 = newest
    
    // Get all elements in order (oldest to newest)
    std::vector<T> getAll() const;
    
    // Get last N elements (most recent)
    std::vector<T> getLast(int n) const;
    
    // Get first N elements (oldest)
    std::vector<T> getFirst(int n) const;
    
    // Status
    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == capacity; }
    int size() const { return count; }
    int getCapacity() const { return capacity; }
    
    // Clear buffer
    void clear();
    
    // Display
    void display() const;
    
    // Statistics (for vital signs monitoring)
    double getAverage() const;  // Only works for numeric types
    T getMin() const;
    T getMax() const;
};

// ==================== IMPLEMENTATION ====================

template<typename T>
CircularBuffer<T>::CircularBuffer(int cap, bool allowOverwrite)
    : capacity(cap), head(0), tail(0), count(0), overwrite(allowOverwrite) {
    buffer.resize(capacity);
    std::cout << "[CIRCULAR-BUFFER] Created with capacity: " << capacity << std::endl;
}

template<typename T>
void CircularBuffer<T>::push(const T& item) {
    if (isFull() && !overwrite) {
        throw std::runtime_error("Buffer is full and overwrite is disabled");
    }
    
    buffer[head] = item;
    head = (head + 1) % capacity;
    
    if (isFull()) {
        // Move tail forward (overwriting oldest)
        tail = (tail + 1) % capacity;
    } else {
        count++;
    }
}

template<typename T>
T CircularBuffer<T>::pop() {
    if (isEmpty()) {
        throw std::runtime_error("Buffer is empty");
    }
    
    T item = buffer[tail];
    tail = (tail + 1) % capacity;
    count--;
    
    return item;
}

template<typename T>
T CircularBuffer<T>::peek() const {
    if (isEmpty()) {
        throw std::runtime_error("Buffer is empty");
    }
    return buffer[tail];
}

template<typename T>
T CircularBuffer<T>::peekAt(int index) const {
    if (index < 0 || index >= count) {
        throw std::out_of_range("Index out of range");
    }
    
    int pos = (tail + index) % capacity;
    return buffer[pos];
}

template<typename T>
std::vector<T> CircularBuffer<T>::getAll() const {
    std::vector<T> result;
    result.reserve(count);
    
    for (int i = 0; i < count; i++) {
        int pos = (tail + i) % capacity;
        result.push_back(buffer[pos]);
    }
    
    return result;
}

template<typename T>
std::vector<T> CircularBuffer<T>::getLast(int n) const {
    if (n > count) n = count;
    
    std::vector<T> result;
    result.reserve(n);
    
    int startIdx = count - n;
    for (int i = startIdx; i < count; i++) {
        int pos = (tail + i) % capacity;
        result.push_back(buffer[pos]);
    }
    
    return result;
}

template<typename T>
std::vector<T> CircularBuffer<T>::getFirst(int n) const {
    if (n > count) n = count;
    
    std::vector<T> result;
    result.reserve(n);
    
    for (int i = 0; i < n; i++) {
        int pos = (tail + i) % capacity;
        result.push_back(buffer[pos]);
    }
    
    return result;
}

template<typename T>
void CircularBuffer<T>::clear() {
    head = 0;
    tail = 0;
    count = 0;
    std::cout << "[CIRCULAR-BUFFER] Cleared" << std::endl;
}

template<typename T>
void CircularBuffer<T>::display() const {
    std::cout << "\n========== Circular Buffer ==========\n";
    std::cout << "Capacity: " << capacity << " | Size: " << count << std::endl;
    std::cout << "Head: " << head << " | Tail: " << tail << std::endl;
    
    if (isEmpty()) {
        std::cout << "Buffer is empty\n";
        return;
    }
    
    std::cout << "\nElements (oldest → newest):\n";
    auto all = getAll();
    for (size_t i = 0; i < all.size(); i++) {
        std::cout << "[" << i << "] ";
        // This assumes T has operator<< or display() method
        if constexpr (std::is_arithmetic<T>::value) {
            std::cout << all[i];
        } else {
            all[i].display();
        }
        std::cout << std::endl;
    }
    std::cout << "====================================\n";
}

template<typename T>
double CircularBuffer<T>::getAverage() const {
    if (isEmpty()) return 0.0;
    
    static_assert(std::is_arithmetic<T>::value, "Average only works for numeric types");
    
    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        int pos = (tail + i) % capacity;
        sum += buffer[pos];
    }
    
    return sum / count;
}

template<typename T>
T CircularBuffer<T>::getMin() const {
    if (isEmpty()) {
        throw std::runtime_error("Buffer is empty");
    }
    
    T minVal = buffer[tail];
    for (int i = 1; i < count; i++) {
        int pos = (tail + i) % capacity;
        if (buffer[pos] < minVal) {
            minVal = buffer[pos];
        }
    }
    
    return minVal;
}

template<typename T>
T CircularBuffer<T>::getMax() const {
    if (isEmpty()) {
        throw std::runtime_error("Buffer is empty");
    }
    
    T maxVal = buffer[tail];
    for (int i = 1; i < count; i++) {
        int pos = (tail + i) % capacity;
        if (buffer[pos] > maxVal) {
            maxVal = buffer[pos];
        }
    }
    
    return maxVal;
}

#endif