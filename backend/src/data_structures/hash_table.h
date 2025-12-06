#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdexcept>

// Generic Hash Table using Chaining for collision resolution
template<typename K, typename V>
class HashTable {
private:
    // Each bucket is a list of key-value pairs
    std::vector<std::list<std::pair<K, V>>> table;
    int tableSize;
    int numElements;
    float maxLoadFactor;
    std::string dataFilePath;
    
    // Hash functions
    int hashFunction(int key) const;
    int hashFunction(const std::string& key) const;
    
    // Helper functions
    void resize();
    float getLoadFactor() const;
    
public:
    // Constructor
    HashTable(int size = 1009, const std::string& filePath = "");
    
    // Destructor
    ~HashTable();
    
    // Main operations
    void insert(const K& key, const V& value);
    V* search(const K& key);
    bool remove(const K& key);
    bool contains(const K& key) const;
    
    // Utility
    int size() const { return numElements; }
    int capacity() const { return tableSize; }
    void display() const;
    void clear();
    
    // Get all keys
    std::vector<K> getAllKeys() const;
    
    // Disk persistence
    void saveToDisk();
    void loadFromDisk();
};

// ==================== IMPLEMENTATION ====================

// Constructor
template<typename K, typename V>
HashTable<K, V>::HashTable(int size, const std::string& filePath) 
    : tableSize(size), numElements(0), maxLoadFactor(0.75), dataFilePath(filePath) {
    table.resize(tableSize);
    
    if (!dataFilePath.empty()) {
        loadFromDisk();
    }
}

// Destructor
template<typename K, typename V>
HashTable<K, V>::~HashTable() {
    if (!dataFilePath.empty()) {
        saveToDisk();
    }
}

// Hash function for integers
template<typename K, typename V>
int HashTable<K, V>::hashFunction(int key) const {
    // Using modulo with prime number for better distribution
    return key % tableSize;
}

// Hash function for strings (djb2 algorithm)
template<typename K, typename V>
int HashTable<K, V>::hashFunction(const std::string& key) const {
    unsigned long hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % tableSize;
}

// Get hash index (template specialization handled in wrapper)
template<typename K, typename V>
int getHashIndex(const K& key, int tableSize) {
    // This will be specialized for int and string
    return 0;
}

// Load factor calculation
template<typename K, typename V>
float HashTable<K, V>::getLoadFactor() const {
    return static_cast<float>(numElements) / tableSize;
}

// Resize table when load factor exceeds threshold
template<typename K, typename V>
void HashTable<K, V>::resize() {
    std::cout << "[HASH] Resizing from " << tableSize;
    
    int oldSize = tableSize;
    tableSize = tableSize * 2 + 1; // Next odd number (preferably prime)
    
    std::cout << " to " << tableSize << std::endl;
    
    // Create new table
    std::vector<std::list<std::pair<K, V>>> newTable(tableSize);
    
    // Rehash all elements
    for (int i = 0; i < oldSize; i++) {
        for (auto& pair : table[i]) {
            int newIndex;
            if constexpr (std::is_same<K, int>::value) {
                newIndex = hashFunction(pair.first);
            } else if constexpr (std::is_same<K, std::string>::value) {
                newIndex = hashFunction(pair.first);
            }
            newTable[newIndex].push_back(pair);
        }
    }
    
    table = std::move(newTable);
}

// Insert key-value pair
template<typename K, typename V>
void HashTable<K, V>::insert(const K& key, const V& value) {
    // Check if key already exists - update if yes
    V* existing = search(key);
    if (existing != nullptr) {
        *existing = value;
        return;
    }
    
    // Calculate hash index
    int index;
    if constexpr (std::is_same<K, int>::value) {
        index = hashFunction(key);
    } else if constexpr (std::is_same<K, std::string>::value) {
        index = hashFunction(key);
    }
    
    // Insert new element
    table[index].push_back({key, value});
    numElements++;
    
    // Check load factor and resize if needed
    if (getLoadFactor() > maxLoadFactor) {
        resize();
    }
}

// Search for key
template<typename K, typename V>
V* HashTable<K, V>::search(const K& key) {
    // Calculate hash index
    int index;
    if constexpr (std::is_same<K, int>::value) {
        index = hashFunction(key);
    } else if constexpr (std::is_same<K, std::string>::value) {
        index = hashFunction(key);
    }
    
    // Search in the bucket
    for (auto& pair : table[index]) {
        if (pair.first == key) {
            return &(pair.second);
        }
    }
    
    return nullptr;
}

// Check if key exists
template<typename K, typename V>
bool HashTable<K, V>::contains(const K& key) const {
    int index;
    if constexpr (std::is_same<K, int>::value) {
        index = hashFunction(key);
    } else if constexpr (std::is_same<K, std::string>::value) {
        index = hashFunction(key);
    }
    
    for (const auto& pair : table[index]) {
        if (pair.first == key) {
            return true;
        }
    }
    
    return false;
}

// Remove key-value pair
template<typename K, typename V>
bool HashTable<K, V>::remove(const K& key) {
    int index;
    if constexpr (std::is_same<K, int>::value) {
        index = hashFunction(key);
    } else if constexpr (std::is_same<K, std::string>::value) {
        index = hashFunction(key);
    }
    
    auto& bucket = table[index];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->first == key) {
            bucket.erase(it);
            numElements--;
            return true;
        }
    }
    
    return false;
}

// Display hash table
template<typename K, typename V>
void HashTable<K, V>::display() const {
    std::cout << "\n========== Hash Table Contents ==========" << std::endl;
    std::cout << "Size: " << numElements << " | Capacity: " << tableSize 
              << " | Load Factor: " << getLoadFactor() << std::endl;
    
    int nonEmptyBuckets = 0;
    int maxChainLength = 0;
    
    for (int i = 0; i < tableSize; i++) {
        if (!table[i].empty()) {
            nonEmptyBuckets++;
            maxChainLength = std::max(maxChainLength, (int)table[i].size());
            
            std::cout << "\nBucket " << i << " (" << table[i].size() << " items): ";
            for (const auto& pair : table[i]) {
                if constexpr (std::is_same<K, int>::value) {
                    std::cout << "[" << pair.first << "] ";
                } else if constexpr (std::is_same<K, std::string>::value) {
                    std::cout << "[" << pair.first << "] ";
                }
            }
        }
    }
    
    std::cout << "\n\nStatistics:" << std::endl;
    std::cout << "Non-empty buckets: " << nonEmptyBuckets << std::endl;
    std::cout << "Max chain length: " << maxChainLength << std::endl;
    std::cout << "========================================\n" << std::endl;
}

// Get all keys
template<typename K, typename V>
std::vector<K> HashTable<K, V>::getAllKeys() const {
    std::vector<K> keys;
    for (const auto& bucket : table) {
        for (const auto& pair : bucket) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

// Clear all elements
template<typename K, typename V>
void HashTable<K, V>::clear() {
    table.clear();
    table.resize(tableSize);
    numElements = 0;
}

// Save to disk
template<typename K, typename V>
void HashTable<K, V>::saveToDisk() {
    if (dataFilePath.empty()) return;
    
    std::ofstream file(dataFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[HASH] Error: Cannot open file for writing: " 
                  << dataFilePath << std::endl;
        return;
    }
    
    // Write metadata
    file.write(reinterpret_cast<const char*>(&tableSize), sizeof(tableSize));
    file.write(reinterpret_cast<const char*>(&numElements), sizeof(numElements));
    
    // Write all key-value pairs
    for (const auto& bucket : table) {
        for (const auto& pair : bucket) {
            // Write key-value pair using their writeToDisk methods
            if constexpr (std::is_same<K, int>::value) {
                file.write(reinterpret_cast<const char*>(&pair.first), sizeof(int));
            } else if constexpr (std::is_same<K, std::string>::value) {
                size_t len = pair.first.length();
                file.write(reinterpret_cast<const char*>(&len), sizeof(len));
                file.write(pair.first.c_str(), len);
            }
            
            pair.second.writeToDisk(file);
        }
    }
    
    file.close();
    std::cout << "[HASH] Saved to " << dataFilePath << " (" 
              << numElements << " elements)" << std::endl;
}

// Load from disk
template<typename K, typename V>
void HashTable<K, V>::loadFromDisk() {
    if (dataFilePath.empty()) return;
    
    std::ifstream file(dataFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "[HASH] No existing data file found: " << dataFilePath << std::endl;
        return;
    }
    
    // Read metadata
    int savedTableSize, savedNumElements;
    file.read(reinterpret_cast<char*>(&savedTableSize), sizeof(savedTableSize));
    file.read(reinterpret_cast<char*>(&savedNumElements), sizeof(savedNumElements));
    
    // Resize table if needed
    if (savedTableSize != tableSize) {
        tableSize = savedTableSize;
        table.resize(tableSize);
    }
    
    // Read all key-value pairs
    for (int i = 0; i < savedNumElements; i++) {
        K key;
        V value;
        
        // Read key
        if constexpr (std::is_same<K, int>::value) {
            file.read(reinterpret_cast<char*>(&key), sizeof(int));
        } else if constexpr (std::is_same<K, std::string>::value) {
            size_t len;
            file.read(reinterpret_cast<char*>(&len), sizeof(len));
            char* buffer = new char[len + 1];
            file.read(buffer, len);
            buffer[len] = '\0';
            key = std::string(buffer);
            delete[] buffer;
        }
        
        // Read value
        value.readFromDisk(file);
        
        // Insert into table
        insert(key, value);
    }
    
    file.close();
    std::cout << "[HASH] Loaded from " << dataFilePath << " (" 
              << numElements << " elements)" << std::endl;
}

#endif