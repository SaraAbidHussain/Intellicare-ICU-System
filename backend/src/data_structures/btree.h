#ifndef BTREE_H
#define BTREE_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "../models/vital_record.h"

class BTreeNode {
public:
    std::vector<long> keys;
    std::vector<VitalRecord> records;
    std::vector<BTreeNode*> children;
    bool isLeaf;
    int minDegree;
    
    BTreeNode(int degree, bool leaf);
    
    void insertNonFull(long key, VitalRecord record);
    void splitChild(int index, BTreeNode* child);
    BTreeNode* search(long key);
    void traverse();
    int findKey(long key);
    void rangeQuery(long startKey, long endKey, std::vector<VitalRecord>& results);
    
    // Disk I/O methods
    void writeToDisk(std::ofstream& file);
    void readFromDisk(std::ifstream& file);
    
    friend class BTree;
};

class BTree {
private:
    BTreeNode* root;
    int minDegree;
    std::string dataFilePath;  // Path to data file
    
    void deleteTree(BTreeNode* node);
    void saveNode(BTreeNode* node, std::ofstream& file);
    BTreeNode* loadNode(std::ifstream& file);
    
public:
    BTree(int degree, const std::string& filePath = "btree_data.bin");
    ~BTree();
    
    void insert(long timestamp, VitalRecord record);
    VitalRecord* search(long timestamp);
    void traverse();
    std::vector<VitalRecord> rangeQuery(long startTime, long endTime);
    
    // Persistence methods
    void saveToDisk();
    void loadFromDisk();
};

#endif