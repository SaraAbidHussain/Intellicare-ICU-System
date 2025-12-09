#include "btree.h"
#include <iostream>
#include <cstring>

// ==================== DiskBTreeNode ====================

DiskBTreeNode::DiskBTreeNode(int degree, bool leaf)
    : isLeaf(leaf), minDegree(degree), numKeys(0), diskPosition(-1) {
    memset(keys, 0, sizeof(keys));
    memset(dataPositions, 0, sizeof(dataPositions));
    memset(childPositions, 0, sizeof(childPositions));
}

size_t DiskBTreeNode::getDiskSize() {
    return sizeof(bool) + sizeof(int) * 2 + 
           sizeof(long) * MAX_KEYS * 2 + 
           sizeof(long) * (MAX_KEYS + 1) +
           sizeof(long);
}

void DiskBTreeNode::writeToDisk(std::ofstream& file) {
    file.write(reinterpret_cast<const char*>(&isLeaf), sizeof(isLeaf));
    file.write(reinterpret_cast<const char*>(&minDegree), sizeof(minDegree));
    file.write(reinterpret_cast<const char*>(&numKeys), sizeof(numKeys));
    file.write(reinterpret_cast<const char*>(keys), sizeof(keys));
    file.write(reinterpret_cast<const char*>(dataPositions), sizeof(dataPositions));
    file.write(reinterpret_cast<const char*>(childPositions), sizeof(childPositions));
    file.write(reinterpret_cast<const char*>(&diskPosition), sizeof(diskPosition));
}

void DiskBTreeNode::readFromDisk(std::ifstream& file) {
    file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));
    file.read(reinterpret_cast<char*>(&minDegree), sizeof(minDegree));
    file.read(reinterpret_cast<char*>(&numKeys), sizeof(numKeys));
    file.read(reinterpret_cast<char*>(keys), sizeof(keys));
    file.read(reinterpret_cast<char*>(dataPositions), sizeof(dataPositions));
    file.read(reinterpret_cast<char*>(childPositions), sizeof(childPositions));
    file.read(reinterpret_cast<char*>(&diskPosition), sizeof(diskPosition));
}

// ==================== DiskBTree ====================

DiskBTree::DiskBTree(int degree, const std::string& basePath)
    : minDegree(degree), rootPosition(0), 
      indexFilePath(basePath + "_index.dat"),
      dataFilePath(basePath + "_data.dat"),
      metaFilePath(basePath + "_meta.dat"),
      nextNodePosition(0), nextDataPosition(0), totalRecords(0) {
    
    // Check if files exist
    std::ifstream testMeta(metaFilePath);
    bool exists = testMeta.good();
    testMeta.close();
    
    if (exists) {
        loadMeta();
        std::cout << "[DISK-BTREE] Loaded existing tree (" << totalRecords << " records)" << std::endl;
    } else {
        // Create new tree
        DiskBTreeNode* root = new DiskBTreeNode(minDegree, true);
        rootPosition = allocateNodePosition();
        root->diskPosition = rootPosition;
        saveNode(root);
        deleteNode(root);
        saveMeta();
        std::cout << "[DISK-BTREE] Created new disk-based B-tree" << std::endl;
    }
}

DiskBTree::~DiskBTree() {
    saveMeta();
}

void DiskBTree::saveMeta() {
    std::ofstream meta(metaFilePath, std::ios::binary);
    meta.write(reinterpret_cast<const char*>(&minDegree), sizeof(minDegree));
    meta.write(reinterpret_cast<const char*>(&rootPosition), sizeof(rootPosition));
    meta.write(reinterpret_cast<const char*>(&nextNodePosition), sizeof(nextNodePosition));
    meta.write(reinterpret_cast<const char*>(&nextDataPosition), sizeof(nextDataPosition));
    meta.write(reinterpret_cast<const char*>(&totalRecords), sizeof(totalRecords));
    meta.close();
}

void DiskBTree::loadMeta() {
    std::ifstream meta(metaFilePath, std::ios::binary);
    meta.read(reinterpret_cast<char*>(&minDegree), sizeof(minDegree));
    meta.read(reinterpret_cast<char*>(&rootPosition), sizeof(rootPosition));
    meta.read(reinterpret_cast<char*>(&nextNodePosition), sizeof(nextNodePosition));
    meta.read(reinterpret_cast<char*>(&nextDataPosition), sizeof(nextDataPosition));
    meta.read(reinterpret_cast<char*>(&totalRecords), sizeof(totalRecords));
    meta.close();
}

long DiskBTree::allocateNodePosition() {
    long pos = nextNodePosition;
    nextNodePosition += DiskBTreeNode::getDiskSize();
    return pos;
}

long DiskBTree::allocateDataPosition() {
    long pos = nextDataPosition;
    nextDataPosition += VitalRecord::getDiskSize();
    return pos;
}

DiskBTreeNode* DiskBTree::loadNode(long position) {
    std::ifstream file(indexFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening index file for reading" << std::endl;
        return nullptr;
    }
    
    file.seekg(position);
    DiskBTreeNode* node = new DiskBTreeNode(minDegree, true);
    node->readFromDisk(file);
    file.close();
    
    return node;
}

void DiskBTree::saveNode(DiskBTreeNode* node) {
    std::ofstream file;
    
    // Check if file exists
    std::ifstream test(indexFilePath);
    bool exists = test.good();
    test.close();
    
    if (exists) {
        file.open(indexFilePath, std::ios::binary | std::ios::in | std::ios::out);
    } else {
        file.open(indexFilePath, std::ios::binary);
    }
    
    if (!file.is_open()) {
        std::cerr << "Error opening index file for writing" << std::endl;
        return;
    }
    
    file.seekp(node->diskPosition);
    node->writeToDisk(file);
    file.close();
}

void DiskBTree::deleteNode(DiskBTreeNode* node) {
    delete node;
}

VitalRecord DiskBTree::loadRecord(long position) {
    std::ifstream file(dataFilePath, std::ios::binary);
    file.seekg(position);
    
    VitalRecord record;
    record.readFromDisk(file);
    record.diskPosition = position;
    
    file.close();
    return record;
}

long DiskBTree::saveRecord(const VitalRecord& record) {
    long position = allocateDataPosition();
    
    std::ofstream file;
    std::ifstream test(dataFilePath);
    bool exists = test.good();
    test.close();
    
    if (exists) {
        file.open(dataFilePath, std::ios::binary | std::ios::in | std::ios::out);
    } else {
        file.open(dataFilePath, std::ios::binary);
    }
    
    file.seekp(position);
    record.writeToDisk(file);
    file.close();
    
    return position;
}

void DiskBTree::insert(long timestamp, const VitalRecord& record) {
    // Save record to data file
    long dataPos = saveRecord(record);
    
    // Load root
    DiskBTreeNode* root = loadNode(rootPosition);
    
    // If root is full, split
    if (root->numKeys == 2 * minDegree - 1) {
        DiskBTreeNode* newRoot = new DiskBTreeNode(minDegree, false);
        newRoot->diskPosition = allocateNodePosition();
        newRoot->childPositions[0] = rootPosition;
        
        splitChild(newRoot, 0);
        
        rootPosition = newRoot->diskPosition;
        saveNode(newRoot);
        
        insertNonFull(newRoot, timestamp, dataPos);
        deleteNode(newRoot);
    } else {
        insertNonFull(root, timestamp, dataPos);
    }
    
    deleteNode(root);
    
    totalRecords++;
    saveMeta();
    
    std::cout << "[DISK-BTREE] Inserted record (total: " << totalRecords << ")" << std::endl;
}

void DiskBTree::insertNonFull(DiskBTreeNode* node, long key, long dataPos) {
    int i = node->numKeys - 1;
    
    if (node->isLeaf) {
        // Shift keys to make room
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            node->dataPositions[i + 1] = node->dataPositions[i];
            i--;
        }
        
        node->keys[i + 1] = key;
        node->dataPositions[i + 1] = dataPos;
        node->numKeys++;
        
        saveNode(node);
    } else {
        // Find child
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;
        
        DiskBTreeNode* child = loadNode(node->childPositions[i]);
        
        if (child->numKeys == 2 * minDegree - 1) {
            splitChild(node, i);
            
            if (node->keys[i] < key) {
                i++;
            }
            
            deleteNode(child);
            child = loadNode(node->childPositions[i]);
        }
        
        insertNonFull(child, key, dataPos);
        deleteNode(child);
    }
}

void DiskBTree::splitChild(DiskBTreeNode* parent, int index) {
    DiskBTreeNode* child = loadNode(parent->childPositions[index]);
    DiskBTreeNode* newChild = new DiskBTreeNode(minDegree, child->isLeaf);
    newChild->diskPosition = allocateNodePosition();
    
    int mid = minDegree - 1;
    newChild->numKeys = minDegree - 1;
    
    // Copy second half to new node
    for (int j = 0; j < minDegree - 1; j++) {
        newChild->keys[j] = child->keys[mid + 1 + j];
        newChild->dataPositions[j] = child->dataPositions[mid + 1 + j];
    }
    
    if (!child->isLeaf) {
        for (int j = 0; j < minDegree; j++) {
            newChild->childPositions[j] = child->childPositions[mid + 1 + j];
        }
    }
    
    child->numKeys = mid;
    
    // Insert middle key into parent
    for (int j = parent->numKeys; j > index; j--) {
        parent->keys[j] = parent->keys[j - 1];
        parent->dataPositions[j] = parent->dataPositions[j - 1];
        parent->childPositions[j + 1] = parent->childPositions[j];
    }
    
    parent->keys[index] = child->keys[mid];
    parent->dataPositions[index] = child->dataPositions[mid];
    parent->childPositions[index + 1] = newChild->diskPosition;
    parent->numKeys++;
    
    saveNode(child);
    saveNode(newChild);
    saveNode(parent);
    
    deleteNode(child);
    deleteNode(newChild);
}
long DiskBTree::searchHelper(DiskBTreeNode* node, long key) {
    int i = 0;
    
    while (i < node->numKeys && key > node->keys[i]) {
        i++;
    }
    
    if (i < node->numKeys && key == node->keys[i]) {
        return node->dataPositions[i];
    }
    
    if (node->isLeaf) {
        return -1;
    }
    
    DiskBTreeNode* child = loadNode(node->childPositions[i]);
    long result = searchHelper(child, key);
    deleteNode(child);
    
    return result;
}
VitalRecord* DiskBTree::search(long timestamp) {
    DiskBTreeNode* root = loadNode(rootPosition);
    long dataPos = searchHelper(root, timestamp);
    deleteNode(root);
    
    if (dataPos != -1) {
        VitalRecord* record = new VitalRecord();
        *record = loadRecord(dataPos);
        return record;
    }
    
    return nullptr;
}

DiskBTreeNode* DiskBTree::searchNode(DiskBTreeNode* node, long key) {
    int i = 0;
    while (i < node->numKeys && key > node->keys[i]) {
        i++;
    }
    
    if (i < node->numKeys && key == node->keys[i]) {
        return node;
    }
    
    if (node->isLeaf) {
        return nullptr;
    }
    
    DiskBTreeNode* child = loadNode(node->childPositions[i]);
    DiskBTreeNode* result = searchNode(child, key);
    
    if (result != child) {
        deleteNode(child);
    }
    
    return result;
}

std::vector<VitalRecord> DiskBTree::rangeQuery(long startTime, long endTime) {
    std::vector<VitalRecord> results;
    DiskBTreeNode* root = loadNode(rootPosition);
    rangeQueryHelper(root, startTime, endTime, results);
    deleteNode(root);
    return results;
}


void DiskBTree::rangeQueryHelper(DiskBTreeNode* node, long startKey, long endKey, 
                                  std::vector<VitalRecord>& results) {
    int i = 0;
    
    while (i < node->numKeys && node->keys[i] < startKey) {
        i++;
    }
    
    for (; i < node->numKeys; i++) {
        if (!node->isLeaf) {
            DiskBTreeNode* child = loadNode(node->childPositions[i]);
            rangeQueryHelper(child, startKey, endKey, results);
            deleteNode(child);
        }
        
        if (node->keys[i] > endKey) {
            return;
        }
        
        if (node->keys[i] >= startKey && node->keys[i] <= endKey) {
            results.push_back(loadRecord(node->dataPositions[i]));
        }
    }
    
    if (!node->isLeaf) {
        DiskBTreeNode* child = loadNode(node->childPositions[i]);
        rangeQueryHelper(child, startKey, endKey, results);
        deleteNode(child);
    }
}