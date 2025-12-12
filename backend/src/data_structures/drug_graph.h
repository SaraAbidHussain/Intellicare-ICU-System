#ifndef DRUG_GRAPH_H
#define DRUG_GRAPH_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <iostream>

// Interaction severity levels
enum InteractionSeverity {
    NONE = 0,
    MILD = 1,
    MODERATE = 2,
    SEVERE = 3,
    CONTRAINDICATED = 4
};

// Drug interaction edge
struct DrugInteraction {
    std::string drug1;
    std::string drug2;
    InteractionSeverity severity;
    std::string description;
    std::vector<std::string> symptoms;  // Potential symptoms of interaction
    
    DrugInteraction();
    DrugInteraction(const std::string& d1, const std::string& d2, 
                    InteractionSeverity sev, const std::string& desc);
    
    void display() const;
    std::string getSeverityString() const;
    
    // Disk I/O
    void writeToDisk(std::ofstream& file) const;
    void readFromDisk(std::ifstream& file);
};

// Graph for drug interactions
class DrugGraph {
private:
    // Adjacency list: drug name -> list of interactions
    std::map<std::string, std::vector<DrugInteraction>> adjList;
    
    // Set of all drug names in graph
    std::set<std::string> drugs;
    
    std::string dataFilePath;
    
    // DFS helper for cycle detection and path finding
    bool dfsHelper(const std::string& current, 
                   const std::string& target,
                   std::set<std::string>& visited,
                   std::vector<std::string>& path,
                   std::vector<DrugInteraction>& interactions);
    
    // DFS for finding all reachable drugs
    void dfsCollectAll(const std::string& start,
                       std::set<std::string>& visited,
                       std::vector<DrugInteraction>& allInteractions);

public:
    // Constructor & Destructor
    DrugGraph(const std::string& filePath = "");
    ~DrugGraph();
    
    // Add drug to graph (node)
    void addDrug(const std::string& drugName);
    
    // Add interaction between two drugs (edge)
    void addInteraction(const std::string& drug1, 
                       const std::string& drug2,
                       InteractionSeverity severity,
                       const std::string& description);
    
    // Check if a drug exists in graph
    bool hasDrug(const std::string& drugName) const;
    
    // Get direct interactions for a drug
    std::vector<DrugInteraction> getDirectInteractions(const std::string& drugName) const;
    
    // Check interaction between two specific drugs
    DrugInteraction* findInteraction(const std::string& drug1, const std::string& drug2);
    
    // Check safety of multiple drugs together (main function)
    struct SafetyReport {
        bool isSafe;
        int totalInteractions;
        std::vector<DrugInteraction> allInteractions;
        InteractionSeverity maxSeverity;
        std::vector<std::string> criticalPairs;
        
        void display() const;
    };
    
    SafetyReport checkDrugCombination(const std::vector<std::string>& medications);
    
    // DFS-based interaction path finding
    bool hasInteractionPath(const std::string& drug1, const std::string& drug2,
                           std::vector<std::string>& path,
                           std::vector<DrugInteraction>& interactions);
    
    // Get all drugs in graph
    std::vector<std::string> getAllDrugs() const;
    
    // Statistics
    int getDrugCount() const { return drugs.size(); }
    int getInteractionCount() const;
    
    // Display
    void display() const;
    void displayDrugInfo(const std::string& drugName) const;
    
    // Disk persistence
    void saveToDisk();
    void loadFromDisk();
    
    // Clear all data
    void clear();
    
    // Populate with common drug interactions (for testing)
    void loadCommonInteractions();
};

#endif