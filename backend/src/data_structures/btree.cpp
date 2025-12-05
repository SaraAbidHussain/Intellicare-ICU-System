#include "btree.h"
#include <algorithm>
#include <cstring>

// ==================== BTreeNode Implementation ====================

BTreeNode::BTreeNode(int degree, bool leaf) {
    minDegree = degree;
    isLeaf = leaf;
}

int BTreeNode::findKey(long key) {
    int index = 0;
    while (index < keys.size() && keys[index] < key) {
        index++;
    }
    return index;
}

void BTreeNode::traverse() {
    int i;
    for (i = 0; i < keys.size(); i++) {
        if (!isLeaf) {
            children[i]->traverse();
        }
        std::cout << "Timestamp: " << keys[i] << " -> ";
        records[i].display();
    }
    
    if (!isLeaf) {
        children[i]->traverse();
    }
}

BTreeNode* BTreeNode::search(long key) {
    int i = findKey(key);
    
    if (i < keys.size() && keys[i] == key) {
        return this;
    }
    
    if (isLeaf) {
        return nullptr;
    }
    
    return children[i]->search(key);
}

void BTreeNode::insertNonFull(long key, VitalRecord record) {
    int i = keys.size() - 1;
    
    if (isLeaf) {
        keys.push_back(0);
        records.push_back(VitalRecord());
        
        while (i >= 0 && keys[i] > key) {
            keys[i + 1] = keys[i];
            records[i + 1] = records[i];
            i--;
        }
        
        keys[i + 1] = key;
        records[i + 1] = record;
    }
    else {
        while (i >= 0 && keys[i] > key) {
            i--;
        }
        i++;
        
        if (children[i]->keys.size() == 2 * minDegree - 1) {
            splitChild(i, children[i]);
            
            if (keys[i] < key) {
                i++;
            }
        }
        
        children[i]->insertNonFull(key, record);
    }
}

void BTreeNode::splitChild(int index, BTreeNode* child) {
    BTreeNode* newNode = new BTreeNode(child->minDegree, child->isLeaf);
    
    int mid = minDegree - 1;
    
    for (int j = 0; j < minDegree - 1; j++) {
        newNode->keys.push_back(child->keys[mid + 1 + j]);
        newNode->records.push_back(child->records[mid + 1 + j]);
    }
    
    if (!child->isLeaf) {
        for (int j = 0; j < minDegree; j++) {
            newNode->children.push_back(child->children[mid + 1 + j]);
        }
    }
    
    child->keys.resize(mid);
    child->records.resize(mid);
    if (!child->isLeaf) {
        child->children.resize(mid + 1);
    }
    
    children.insert(children.begin() + index + 1, newNode);
    keys.insert(keys.begin() + index, child->keys[mid]);
    records.insert(records.begin() + index, child->records[mid]);
}

void BTreeNode::rangeQuery(long startKey, long endKey, std::vector<VitalRecord>& results) {
    int i = 0;
    
    while (i < keys.size() && keys[i] < startKey) {
        i++;
    }
    
    for (; i < keys.size(); i++) {
        if (!isLeaf) {
            children[i]->rangeQuery(startKey, endKey, results);
        }
        
        if (keys[i] >= startKey && keys[i] <= endKey) {
            results.push_back(records[i]);
        }
        
        if (keys[i] > endKey) {
            return;
        }
    }
    
    if (!isLeaf && i < children.size()) {
        children[i]->rangeQuery(startKey, endKey, results);
    }
}

// Write node to disk
void BTreeNode::writeToDisk(std::ofstream& file) {
    // Write node metadata
    file.write(reinterpret_cast<const char*>(&isLeaf), sizeof(isLeaf));
    file.write(reinterpret_cast<const char*>(&minDegree), sizeof(minDegree));
    
    // Write number of keys
    int numKeys = keys.size();
    file.write(reinterpret_cast<const char*>(&numKeys), sizeof(numKeys));
    
    // Write keys
    for (int i = 0; i < numKeys; i++) {
        file.write(reinterpret_cast<const char*>(&keys[i]), sizeof(long));
    }
    
    // Write records
    for (int i = 0; i < numKeys; i++) {
        records[i].writeToDisk(file);
    }
    
    // Write number of children
    int numChildren = children.size();
    file.write(reinterpret_cast<const char*>(&numChildren), sizeof(numChildren));
}

// Read node from disk
void BTreeNode::readFromDisk(std::ifstream& file) {
    // Read node metadata
    file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));
    file.read(reinterpret_cast<char*>(&minDegree), sizeof(minDegree));
    
    // Read number of keys
    int numKeys;
    file.read(reinterpret_cast<char*>(&numKeys), sizeof(numKeys));
    
    // Read keys
    keys.clear();
    for (int i = 0; i < numKeys; i++) {
        long key;
        file.read(reinterpret_cast<char*>(&key), sizeof(long));
        keys.push_back(key);
    }
    
    // Read records
    records.clear();
    for (int i = 0; i < numKeys; i++) {
        VitalRecord record;
        record.readFromDisk(file);
        records.push_back(record);
    }
    
    // Read number of children (we'll create children pointers later)
    int numChildren;
    file.read(reinterpret_cast<char*>(&numChildren), sizeof(numChildren));
}

// ==================== BTree Implementation ====================

BTree::BTree(int degree, const std::string& filePath) {
    minDegree = degree;
    dataFilePath = filePath;
    root = nullptr;
    
    // Try to load existing data from disk
    loadFromDisk();
}

BTree::~BTree() {
    saveToDisk();  // Save before destroying
    deleteTree(root);
}

void BTree::deleteTree(BTreeNode* node) {
    if (node == nullptr) return;
    
    if (!node->isLeaf) {
        for (auto child : node->children) {
            deleteTree(child);
        }
    }
    
    delete node;
}

void BTree::insert(long timestamp, VitalRecord record) {
    if (root == nullptr) {
        root = new BTreeNode(minDegree, true);
        root->keys.push_back(timestamp);
        root->records.push_back(record);
        saveToDisk();  // Save after insert
        return;
    }
    
    if (root->keys.size() == 2 * minDegree - 1) {
        BTreeNode* newRoot = new BTreeNode(minDegree, false);
        newRoot->children.push_back(root);
        newRoot->splitChild(0, root);
        
        int i = 0;
        if (newRoot->keys[0] < timestamp) {
            i++;
        }
        newRoot->children[i]->insertNonFull(timestamp, record);
        
        root = newRoot;
    }
    else {
        root->insertNonFull(timestamp, record);
    }
    
    saveToDisk();  // Save after every insert
}

VitalRecord* BTree::search(long timestamp) {
    if (root == nullptr) {
        return nullptr;
    }
    
    BTreeNode* result = root->search(timestamp);
    if (result == nullptr) {
        return nullptr;
    }
    
    for (int i = 0; i < result->keys.size(); i++) {
        if (result->keys[i] == timestamp) {
            return &(result->records[i]);
        }
    }
    
    return nullptr;
}

void BTree::traverse() {
    if (root != nullptr) {
        root->traverse();
    }
}

std::vector<VitalRecord> BTree::rangeQuery(long startTime, long endTime) {
    std::vector<VitalRecord> results;
    
    if (root == nullptr) {
        return results;
    }
    
    root->rangeQuery(startTime, endTime, results);
    return results;
}

// Save entire tree to disk
void BTree::saveToDisk() {
    std::ofstream file(dataFilePath, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file for writing: " << dataFilePath << std::endl;
        return;
    }
    
    // Write tree metadata
    file.write(reinterpret_cast<const char*>(&minDegree), sizeof(minDegree));
    
    // Write if root exists
    bool hasRoot = (root != nullptr);
    file.write(reinterpret_cast<const char*>(&hasRoot), sizeof(hasRoot));
    
    if (hasRoot) {
        saveNode(root, file);
    }
    
    file.close();
    std::cout << "[DISK] Tree saved to " << dataFilePath << std::endl;
}

// Recursive function to save nodes
void BTree::saveNode(BTreeNode* node, std::ofstream& file) {
    if (node == nullptr) return;
    
    // Write current node
    node->writeToDisk(file);
    
    // Recursively write children
    if (!node->isLeaf) {
        for (auto child : node->children) {
            saveNode(child, file);
        }
    }
}

// Load tree from disk
void BTree::loadFromDisk() {
    std::ifstream file(dataFilePath, std::ios::binary);
    
    if (!file.is_open()) {
        std::cout << "[DISK] No existing data file found. Starting fresh." << std::endl;
        return;
    }
    
    // Read tree metadata
    file.read(reinterpret_cast<char*>(&minDegree), sizeof(minDegree));
    
    // Read if root exists
    bool hasRoot;
    file.read(reinterpret_cast<char*>(&hasRoot), sizeof(hasRoot));
    
    if (hasRoot) {
        root = loadNode(file);
        std::cout << "[DISK] Tree loaded from " << dataFilePath << std::endl;
    }
    
    file.close();
}

// Recursive function to load nodes
BTreeNode* BTree::loadNode(std::ifstream& file) {
    BTreeNode* node = new BTreeNode(minDegree, true);
    node->readFromDisk(file);
    
    // If node has children, load them recursively
    if (!node->isLeaf) {
        int numChildren = node->children.size(); // This was stored during write
        node->children.clear();
        
        // We need to track how many children to load
        // Get it from the file (we stored numChildren during write)
        file.seekg(-sizeof(int), std::ios::cur); // Go back to read numChildren again
        int childCount;
        file.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));
        
        for (int i = 0; i < childCount; i++) {
            BTreeNode* child = loadNode(file);
            node->children.push_back(child);
        }
    }
    
    return node;
}