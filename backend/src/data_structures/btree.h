#ifndef BTREE_DISK_H
#define BTREE_DISK_H

#include <vector>
#include <string>
#include <fstream>
#include <map>
#include "../models/vital_record.h"

// Maximum keys per node (for fixed-size disk allocation)
const int MAX_KEYS = 99;  // For degree 50

struct DiskBTreeNode {
    bool isLeaf;
    int minDegree;
    int numKeys;
    long keys[MAX_KEYS];
    long dataPositions[MAX_KEYS];
    long childPositions[MAX_KEYS + 1];
    long diskPosition;
    
    DiskBTreeNode(int degree, bool leaf);
    
    static size_t getDiskSize();
    void writeToDisk(std::ofstream& file);
    void readFromDisk(std::ifstream& file);
};

class DiskBTree {
private:
    int minDegree;
    long rootPosition;
    std::string indexFilePath;
    std::string dataFilePath;
    std::string metaFilePath;
    
    // Metadata
    long nextNodePosition;
    long nextDataPosition;
    int totalRecords;
    
    // Helper functions
    DiskBTreeNode* loadNode(long position);
    void saveNode(DiskBTreeNode* node);
    void deleteNode(DiskBTreeNode* node);
    
    long allocateNodePosition();
    long allocateDataPosition();
    
    void insertNonFull(DiskBTreeNode* node, long key, long dataPos);
    void splitChild(DiskBTreeNode* parent, int index);
    
    DiskBTreeNode* searchNode(DiskBTreeNode* node, long key);
    void rangeQueryHelper(DiskBTreeNode* node, long startKey, long endKey, 
                         std::vector<VitalRecord>& results);
    
    void saveMeta();
    void loadMeta();
    
    VitalRecord loadRecord(long position);
    long saveRecord(const VitalRecord& record);
    long searchHelper(DiskBTreeNode* node, long key);
    
public:
    DiskBTree(int degree, const std::string& basePath);
    ~DiskBTree();
    
    void insert(long timestamp, const VitalRecord& record);
    VitalRecord* search(long timestamp);
    std::vector<VitalRecord> rangeQuery(long startTime, long endTime);
    
    int getRecordCount() const { return totalRecords; }
};

#endif