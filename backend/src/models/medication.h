#ifndef MEDICATION_H
#define MEDICATION_H

#include <string>
#include <vector>
#include <fstream>

struct Medication {
    std::string drugName;
    std::string genericName;
    std::string category;  // e.g., "Antibiotic", "Analgesic"
    std::string dosageForm;  // e.g., "Tablet", "Injection"
    int standardDosage;  // in mg
    std::vector<std::string> sideEffects;
    std::vector<std::string> contraindications;
    bool requiresPrescription;
    
    // Constructors
    Medication();
    Medication(const std::string& name, const std::string& generic, 
               const std::string& cat, int dosage);
    
    // Display
    void display() const;
    
    // Disk I/O
    void writeToDisk(std::ofstream& file) const;
    void readFromDisk(std::ifstream& file);
};

#endif