//file structure: backend/src/data_structures/kdtree.h
#ifndef KDTREE_H
#define KDTREE_H

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include "../models/vital_record.h"

// Point in K-dimensional space (for vitals)
struct VitalPoint {
    int patientID;
    std::vector<double> coordinates;  // [heart_rate, systolic_bp, diastolic_bp, spo2, temperature]
    long timestamp;
    
    VitalPoint();
    VitalPoint(const VitalRecord& record);
    VitalPoint(int pid, const std::vector<double>& coords, long ts);
    
    // Calculate distance to another point
    double distanceTo(const VitalPoint& other) const;
    
    void display() const;

    bool operator<(const VitalPoint& other) const {
        // Compare by patient ID (arbitrary but consistent)
        return patientID < other.patientID;
    }
    
    bool operator>(const VitalPoint& other) const {
        return patientID > other.patientID;
    }
    
    bool operator==(const VitalPoint& other) const {
        return patientID == other.patientID;
    }
};

// KD-Tree Node
struct KDNode {
    VitalPoint point;
    KDNode* left;
    KDNode* right;
    int splitDimension;  // Which dimension this node splits on
    
    KDNode(const VitalPoint& p, int dim);
    ~KDNode();
};

// KD-Tree for clustering patients by similar vitals
class KDTree {
private:
    KDNode* root;
    int dimensions;  // Number of vital signs (5: HR, SBP, DBP, SpO2, Temp)
    int totalPoints;
    std::string dataFilePath;
    
    // Recursive build helper
    KDNode* buildTree(std::vector<VitalPoint>& points, int depth);
    
    // Recursive insert helper
    KDNode* insertHelper(KDNode* node, const VitalPoint& point, int depth);
    
    // Recursive nearest neighbor search
    void nearestNeighborHelper(KDNode* node, const VitalPoint& target, int depth,
                               VitalPoint& best, double& bestDist);
    
    // Recursive k-nearest neighbors search
    void kNearestHelper(KDNode* node, const VitalPoint& target, int depth,
                       std::vector<std::pair<double, VitalPoint>>& neighbors, int k);
    
    // Recursive range search
    void rangeSearchHelper(KDNode* node, const VitalPoint& center, double radius,
                          int depth, std::vector<VitalPoint>& results);
    
    // Tree traversal for disk I/O
    void serializeHelper(KDNode* node, std::ofstream& file);
    KDNode* deserializeHelper(std::ifstream& file);
    
    // Cleanup
    void clearTree(KDNode* node);
    
    // Count nodes
    int countNodes(KDNode* node) const;

public:
    // Constructor & Destructor
    KDTree(int dims = 5, const std::string& filePath = "");
    ~KDTree();
    
    // Build tree from batch of points
    void build(std::vector<VitalPoint>& points);
    
    // Insert single point
    void insert(const VitalPoint& point);
    void insert(const VitalRecord& record);
    
    // Find nearest neighbor
    VitalPoint findNearest(const VitalPoint& target);
    VitalPoint findNearest(const VitalRecord& targetRecord);
    
    // Find k nearest neighbors
    std::vector<VitalPoint> findKNearest(const VitalPoint& target, int k);
    std::vector<VitalPoint> findKNearest(const VitalRecord& targetRecord, int k);
    
    // Find all points within radius
    std::vector<VitalPoint> rangeSearch(const VitalPoint& center, double radius);
    
    // Find similar patients (wrapper for k-nearest)
    struct SimilarPatient {
        int patientID;
        double similarity;  // Distance (lower = more similar)
        VitalPoint vitals;
        
        void display() const;
    };
    
    std::vector<SimilarPatient> findSimilarPatients(const VitalRecord& patientVitals, int k);
    std::vector<SimilarPatient> findSimilarPatients(int patientID, 
                                                    const std::vector<VitalRecord>& allRecords,
                                                    int k);
    
    // Statistics
    int size() const { return totalPoints; }
    int getDimensions() const { return dimensions; }
    bool isEmpty() const { return root == nullptr; }
    
    // Display
    void display() const;
    
    // Persistence
    void saveToDisk();
    void loadFromDisk();
    
    // Clear tree
    void clear();
};

#endif