#include "drug_graph.h"
#include <algorithm>
#include <iomanip>

// ==================== DrugInteraction Implementation ====================

DrugInteraction::DrugInteraction() 
    : drug1(""), drug2(""), severity(NONE), description("") {}

DrugInteraction::DrugInteraction(const std::string& d1, const std::string& d2,
                                InteractionSeverity sev, const std::string& desc)
    : drug1(d1), drug2(d2), severity(sev), description(desc) {}

std::string DrugInteraction::getSeverityString() const {
    switch(severity) {
        case NONE: return "⚪ None";
        case MILD: return "🟢 Mild";
        case MODERATE: return "🟡 Moderate";
        case SEVERE: return "🟠 Severe";
        case CONTRAINDICATED: return "🔴 Contraindicated";
        default: return "Unknown";
    }
}

void DrugInteraction::display() const {
    std::cout << "  ⚠️  " << drug1 << " ↔️ " << drug2 << std::endl;
    std::cout << "      Severity: " << getSeverityString() << std::endl;
    std::cout << "      " << description << std::endl;
    if (!symptoms.empty()) {
        std::cout << "      Symptoms: ";
        for (size_t i = 0; i < symptoms.size(); i++) {
            std::cout << symptoms[i];
            if (i < symptoms.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
}

void DrugInteraction::writeToDisk(std::ofstream& file) const {
    auto writeString = [&](const std::string& str) {
        size_t len = str.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(str.c_str(), len);
    };
    
    writeString(drug1);
    writeString(drug2);
    file.write(reinterpret_cast<const char*>(&severity), sizeof(severity));
    writeString(description);
    
    // Write symptoms vector
    size_t symptomsSize = symptoms.size();
    file.write(reinterpret_cast<const char*>(&symptomsSize), sizeof(symptomsSize));
    for (const auto& symptom : symptoms) {
        writeString(symptom);
    }
}

void DrugInteraction::readFromDisk(std::ifstream& file) {
    auto readString = [&]() {
        size_t len;
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        char* buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        std::string str(buffer);
        delete[] buffer;
        return str;
    };
    
    drug1 = readString();
    drug2 = readString();
    file.read(reinterpret_cast<char*>(&severity), sizeof(severity));
    description = readString();
    
    // Read symptoms vector
    size_t symptomsSize;
    file.read(reinterpret_cast<char*>(&symptomsSize), sizeof(symptomsSize));
    symptoms.clear();
    for (size_t i = 0; i < symptomsSize; i++) {
        symptoms.push_back(readString());
    }
}

// ==================== DrugGraph Implementation ====================

DrugGraph::DrugGraph(const std::string& filePath) 
    : dataFilePath(filePath) {
    if (!dataFilePath.empty()) {
        loadFromDisk();
    }
    
    if (drugs.empty()) {
        std::cout << "[DRUG-GRAPH] Initialized empty drug interaction graph" << std::endl;
    }
}

DrugGraph::~DrugGraph() {
    if (!dataFilePath.empty()) {
        saveToDisk();
    }
}

void DrugGraph::addDrug(const std::string& drugName) {
    if (drugs.find(drugName) == drugs.end()) {
        drugs.insert(drugName);
        adjList[drugName] = std::vector<DrugInteraction>();
        std::cout << "[DRUG-GRAPH] Added drug: " << drugName << std::endl;
    }
}

void DrugGraph::addInteraction(const std::string& drug1, const std::string& drug2,
                              InteractionSeverity severity, const std::string& description) {
    // Add both drugs if they don't exist
    addDrug(drug1);
    addDrug(drug2);
    
    // Create interaction
    DrugInteraction interaction(drug1, drug2, severity, description);
    
    // Add to both adjacency lists (undirected graph)
    adjList[drug1].push_back(interaction);
    
    // Also add reverse for symmetric lookup
    DrugInteraction reverseInteraction(drug2, drug1, severity, description);
    adjList[drug2].push_back(reverseInteraction);
    
    std::cout << "[DRUG-GRAPH] Added interaction: " << drug1 << " ↔️ " << drug2 
              << " (" << interaction.getSeverityString() << ")" << std::endl;
}

bool DrugGraph::hasDrug(const std::string& drugName) const {
    return drugs.find(drugName) != drugs.end();
}

std::vector<DrugInteraction> DrugGraph::getDirectInteractions(const std::string& drugName) const {
    auto it = adjList.find(drugName);
    if (it != adjList.end()) {
        return it->second;
    }
    return std::vector<DrugInteraction>();
}

DrugInteraction* DrugGraph::findInteraction(const std::string& drug1, const std::string& drug2) {
    auto it = adjList.find(drug1);
    if (it != adjList.end()) {
        for (auto& interaction : it->second) {
            if (interaction.drug2 == drug2) {
                return &interaction;
            }
        }
    }
    return nullptr;
}

// Main function: Check safety of drug combination
DrugGraph::SafetyReport DrugGraph::checkDrugCombination(const std::vector<std::string>& medications) {
    SafetyReport report;
    report.isSafe = true;
    report.totalInteractions = 0;
    report.maxSeverity = NONE;
    
    std::cout << "\n[DRUG-GRAPH] Checking combination of " << medications.size() << " drugs..." << std::endl;
    
    // Check all pairs of medications
    for (size_t i = 0; i < medications.size(); i++) {
        const std::string& drug1 = medications[i];
        
        if (!hasDrug(drug1)) {
            std::cout << "  ⚠️  Warning: " << drug1 << " not in database" << std::endl;
            continue;
        }
        
        for (size_t j = i + 1; j < medications.size(); j++) {
            const std::string& drug2 = medications[j];
            
            if (!hasDrug(drug2)) {
                std::cout << "  ⚠️  Warning: " << drug2 << " not in database" << std::endl;
                continue;
            }
            
            // Use DFS to find interaction path
            std::vector<std::string> path;
            std::vector<DrugInteraction> interactions;
            
            if (hasInteractionPath(drug1, drug2, path, interactions)) {
                report.totalInteractions++;
                
                // Add all interactions found in path
                for (const auto& interaction : interactions) {
                    report.allInteractions.push_back(interaction);
                    
                    // Update max severity
                    if (interaction.severity > report.maxSeverity) {
                        report.maxSeverity = interaction.severity;
                    }
                    
                    // Mark critical pairs
                    if (interaction.severity >= SEVERE) {
                        std::string pair = interaction.drug1 + " + " + interaction.drug2;
                        report.criticalPairs.push_back(pair);
                        report.isSafe = false;
                    }
                }
            }
        }
    }
    
    std::cout << "[DRUG-GRAPH] Check complete: " << report.totalInteractions 
              << " interactions found" << std::endl;
    
    return report;
}

// DFS helper for finding interaction path
bool DrugGraph::dfsHelper(const std::string& current, const std::string& target,
                         std::set<std::string>& visited, std::vector<std::string>& path,
                         std::vector<DrugInteraction>& interactions) {
    visited.insert(current);
    path.push_back(current);
    
    // If we reached target, we found a path
    if (current == target) {
        return true;
    }
    
    // Explore neighbors
    auto it = adjList.find(current);
    if (it != adjList.end()) {
        for (const auto& interaction : it->second) {
            const std::string& neighbor = interaction.drug2;
            
            if (visited.find(neighbor) == visited.end()) {
                interactions.push_back(interaction);
                
                if (dfsHelper(neighbor, target, visited, path, interactions)) {
                    return true;
                }
                
                interactions.pop_back();
            }
        }
    }
    
    path.pop_back();
    return false;
}

// Public DFS interface
bool DrugGraph::hasInteractionPath(const std::string& drug1, const std::string& drug2,
                                  std::vector<std::string>& path,
                                  std::vector<DrugInteraction>& interactions) {
    std::set<std::string> visited;
    path.clear();
    interactions.clear();
    
    return dfsHelper(drug1, drug2, visited, path, interactions);
}

std::vector<std::string> DrugGraph::getAllDrugs() const {
    std::vector<std::string> result;
    for (const auto& drug : drugs) {
        result.push_back(drug);
    }
    return result;
}

int DrugGraph::getInteractionCount() const {
    int count = 0;
    for (const auto& pair : adjList) {
        count += pair.second.size();
    }
    return count / 2;  // Divide by 2 because graph is undirected
}

void DrugGraph::display() const {
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║        DRUG INTERACTION GRAPH                      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\nDrugs: " << drugs.size() << " | Interactions: " << getInteractionCount() << "\n" << std::endl;
    
    for (const auto& drug : drugs) {
        displayDrugInfo(drug);
    }
}

void DrugGraph::displayDrugInfo(const std::string& drugName) const {
    auto it = adjList.find(drugName);
    if (it == adjList.end()) {
        std::cout << drugName << ": No interactions recorded" << std::endl;
        return;
    }
    
    std::cout << "📋 " << drugName << " (" << it->second.size() << " interactions)" << std::endl;
    for (const auto& interaction : it->second) {
        std::cout << "    └─ " << interaction.drug2 
                  << " [" << interaction.getSeverityString() << "]" << std::endl;
    }
    std::cout << std::endl;
}

void DrugGraph::SafetyReport::display() const {
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           DRUG SAFETY REPORT                       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "\nOverall Status: " << (isSafe ? "✅ SAFE" : "❌ UNSAFE") << std::endl;
    std::cout << "Total Interactions: " << totalInteractions << std::endl;
    
    if (totalInteractions > 0) {
        DrugInteraction temp;
        temp.severity = maxSeverity;
        std::cout << "Maximum Severity: " << temp.getSeverityString() << std::endl;
    }
    
    if (!criticalPairs.empty()) {
        std::cout << "\n⚠️  CRITICAL DRUG PAIRS:" << std::endl;
        for (const auto& pair : criticalPairs) {
            std::cout << "  • " << pair << std::endl;
        }
    }
    
    if (!allInteractions.empty()) {
        std::cout << "\n📋 DETAILED INTERACTIONS:" << std::endl;
        for (const auto& interaction : allInteractions) {
            interaction.display();
            std::cout << std::endl;
        }
    }
    
    std::cout << "════════════════════════════════════════════════════\n" << std::endl;
}

void DrugGraph::saveToDisk() {
    if (dataFilePath.empty()) return;
    
    std::ofstream file(dataFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[DRUG-GRAPH] Error: Cannot open file for writing: " 
                  << dataFilePath << std::endl;
        return;
    }
    
    // Write number of drugs
    int numDrugs = drugs.size();
    file.write(reinterpret_cast<const char*>(&numDrugs), sizeof(numDrugs));
    
    // Write all drug names
    for (const auto& drug : drugs) {
        size_t len = drug.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(drug.c_str(), len);
    }
    
    // Write number of unique interactions
    int numInteractions = getInteractionCount();
    file.write(reinterpret_cast<const char*>(&numInteractions), sizeof(numInteractions));
    
    // Write all interactions (only once per pair)
    std::set<std::pair<std::string, std::string>> written;
    for (const auto& pair : adjList) {
        for (const auto& interaction : pair.second) {
            std::string d1 = interaction.drug1;
            std::string d2 = interaction.drug2;
            if (d1 > d2) std::swap(d1, d2);
            
            auto key = std::make_pair(d1, d2);
            if (written.find(key) == written.end()) {
                interaction.writeToDisk(file);
                written.insert(key);
            }
        }
    }
    
    file.close();
    std::cout << "[DRUG-GRAPH] Saved " << numDrugs << " drugs and " 
              << numInteractions << " interactions to " << dataFilePath << std::endl;
}

void DrugGraph::loadFromDisk() {
    if (dataFilePath.empty()) return;
    
    std::ifstream file(dataFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "[DRUG-GRAPH] No existing data file found: " << dataFilePath << std::endl;
        return;
    }
    
    clear();
    
    // Read number of drugs
    int numDrugs;
    file.read(reinterpret_cast<char*>(&numDrugs), sizeof(numDrugs));
    
    // Read drug names
    for (int i = 0; i < numDrugs; i++) {
        size_t len;
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        char* buffer = new char[len + 1];
        file.read(buffer, len);
        buffer[len] = '\0';
        std::string drugName(buffer);
        delete[] buffer;
        
        addDrug(drugName);
    }
    
    // Read interactions
    int numInteractions;
    file.read(reinterpret_cast<char*>(&numInteractions), sizeof(numInteractions));
    
    for (int i = 0; i < numInteractions; i++) {
        DrugInteraction interaction;
        interaction.readFromDisk(file);
        addInteraction(interaction.drug1, interaction.drug2, 
                      interaction.severity, interaction.description);
    }
    
    file.close();
    std::cout << "[DRUG-GRAPH] Loaded " << numDrugs << " drugs and " 
              << numInteractions << " interactions from " << dataFilePath << std::endl;
}

void DrugGraph::clear() {
    drugs.clear();
    adjList.clear();
    std::cout << "[DRUG-GRAPH] Graph cleared" << std::endl;
}

// Load common drug interactions for testing
void DrugGraph::loadCommonInteractions() {
    std::cout << "[DRUG-GRAPH] Loading common drug interactions..." << std::endl;
    
    // Warfarin interactions
    addInteraction("Warfarin", "Aspirin", SEVERE, 
                  "Increased bleeding risk");
    addInteraction("Warfarin", "Ibuprofen", SEVERE, 
                  "Increased bleeding risk");
    addInteraction("Warfarin", "Vitamin K", CONTRAINDICATED, 
                  "Antagonizes anticoagulant effect");
    
    // ACE inhibitor interactions
    addInteraction("Lisinopril", "Potassium", SEVERE, 
                  "Risk of hyperkalemia");
    addInteraction("Lisinopril", "Spironolactone", SEVERE, 
                  "Risk of hyperkalemia");
    
    // Antibiotic interactions
    addInteraction("Ciprofloxacin", "Calcium", MODERATE, 
                  "Reduced antibiotic absorption");
    addInteraction("Ciprofloxacin", "Antacids", MODERATE, 
                  "Reduced antibiotic effectiveness");
    
    // Statin interactions
    addInteraction("Atorvastatin", "Grapefruit", MODERATE, 
                  "Increased statin levels, muscle damage risk");
    addInteraction("Atorvastatin", "Clarithromycin", SEVERE, 
                  "Risk of rhabdomyolysis");
    
    // Diabetes medication interactions
    addInteraction("Metformin", "Alcohol", MODERATE, 
                  "Risk of lactic acidosis");
    addInteraction("Insulin", "Beta-blockers", MODERATE, 
                  "Masks hypoglycemia symptoms");
    
    // Common pain medication interactions
    addInteraction("Aspirin", "Ibuprofen", MODERATE, 
                  "Reduced cardioprotective effect");
    addInteraction("Acetaminophen", "Alcohol", SEVERE, 
                  "Increased liver toxicity risk");
    
    // Antidepressant interactions
    addInteraction("Sertraline", "Tramadol", SEVERE, 
                  "Risk of serotonin syndrome");
    addInteraction("Sertraline", "St. John's Wort", CONTRAINDICATED, 
                  "Severe serotonin syndrome risk");
    
    std::cout << "[DRUG-GRAPH] Loaded " << getInteractionCount() 
              << " common interactions" << std::endl;
}
