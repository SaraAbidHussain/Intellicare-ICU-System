#include "patient.h"
#include <iostream>
#include <iomanip>

Patient::Patient() 
    : patientID(0), name(""), age(0), gender('M'), ward(""), 
      admissionDate(""), condition(""), bloodType("") {}

Patient::Patient(int id, const std::string& n, int a, char g, 
                 const std::string& w, const std::string& ad, const std::string& cond)
    : patientID(id), name(n), age(a), gender(g), ward(w), 
      admissionDate(ad), condition(cond), bloodType("") {}

void Patient::display() const {
    std::cout << "┌─────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Patient ID: " << std::setw(4) << patientID 
              << "                          │" << std::endl;
    std::cout << "│ Name: " << std::left << std::setw(33) << name << "│" << std::endl;
    std::cout << "│ Age: " << age << " | Gender: " << gender 
              << " | Blood: " << std::setw(15) << bloodType << "│" << std::endl;
    std::cout << "│ Ward: " << std::setw(33) << ward << "│" << std::endl;
    std::cout << "│ Condition: " << std::setw(27) << condition << "│" << std::endl;
    
    if (!medications.empty()) {
        std::cout << "│ Medications: " << std::setw(26) << medications[0] << "│" << std::endl;
        for (size_t i = 1; i < medications.size(); i++) {
            std::cout << "│              " << std::setw(26) << medications[i] << "│" << std::endl;
        }
    }
    
    if (!allergies.empty()) {
        std::cout << "│ Allergies: " << std::setw(28) << allergies[0] << "│" << std::endl;
        for (size_t i = 1; i < allergies.size(); i++) {
            std::cout << "│            " << std::setw(28) << allergies[i] << "│" << std::endl;
        }
    }
    
    std::cout << "└─────────────────────────────────────────┘" << std::endl;
}

void Patient::addMedication(const std::string& med) {
    medications.push_back(med);
}

void Patient::addAllergy(const std::string& allergy) {
    allergies.push_back(allergy);
}

void Patient::writeToDisk(std::ofstream& file) const {
    // Write primitive types
    file.write(reinterpret_cast<const char*>(&patientID), sizeof(patientID));
    file.write(reinterpret_cast<const char*>(&age), sizeof(age));
    file.write(reinterpret_cast<const char*>(&gender), sizeof(gender));
    
    // Write strings (length + data)
    auto writeString = [&](const std::string& str) {
        size_t len = str.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(str.c_str(), len);
    };
    
    writeString(name);
    writeString(ward);
    writeString(admissionDate);
    writeString(condition);
    writeString(bloodType);
    
    // Write vectors
    auto writeVector = [&](const std::vector<std::string>& vec) {
        size_t size = vec.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& str : vec) {
            writeString(str);
        }
    };
    
    writeVector(medications);
    writeVector(allergies);
}

void Patient::readFromDisk(std::ifstream& file) {
    // Read primitive types
    file.read(reinterpret_cast<char*>(&patientID), sizeof(patientID));
    file.read(reinterpret_cast<char*>(&age), sizeof(age));
    file.read(reinterpret_cast<char*>(&gender), sizeof(gender));
    
    // Read strings
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
    
    name = readString();
    ward = readString();
    admissionDate = readString();
    condition = readString();
    bloodType = readString();
    
    // Read vectors
    auto readVector = [&]() {
        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::vector<std::string> vec;
        for (size_t i = 0; i < size; i++) {
            vec.push_back(readString());
        }
        return vec;
    };
    
    medications = readVector();
    allergies = readVector();
}