#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <vector>
#include <fstream>

struct Patient {
    int patientID;
    std::string name;
    int age;
    char gender;  // 'M' or 'F'
    std::string ward;  // e.g., "ICU-A"
    std::string admissionDate;
    std::string condition;
    std::vector<std::string> medications;
    std::vector<std::string> allergies;
    std::string bloodType;
    
    // Constructors
    Patient();
    Patient(int id, const std::string& n, int a, char g, const std::string& w,
            const std::string& ad, const std::string& cond);
    
    // Display
    void display() const;
    
    // Disk I/O
    void writeToDisk(std::ofstream& file) const;
    void readFromDisk(std::ifstream& file);
    
    // Utility
    void addMedication(const std::string& med);
    void addAllergy(const std::string& allergy);
};

#endif