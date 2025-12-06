#include "medication.h"
#include <iostream>
#include <iomanip>

Medication::Medication() 
    : drugName(""), genericName(""), category(""), dosageForm(""),
      standardDosage(0), requiresPrescription(true) {}

Medication::Medication(const std::string& name, const std::string& generic,
                       const std::string& cat, int dosage)
    : drugName(name), genericName(generic), category(cat), dosageForm("Tablet"),
      standardDosage(dosage), requiresPrescription(true) {}

void Medication::display() const {
    std::cout << "┌─────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Drug: " << std::left << std::setw(34) << drugName << "│" << std::endl;
    std::cout << "│ Generic: " << std::setw(30) << genericName << "│" << std::endl;
    std::cout << "│ Category: " << std::setw(29) << category << "│" << std::endl;
    std::cout << "│ Dosage: " << standardDosage << " mg (" << dosageForm << ")" 
              << std::string(20, ' ') << "│" << std::endl;
    std::cout << "│ Rx Required: " << (requiresPrescription ? "Yes" : "No") 
              << std::string(23, ' ') << "│" << std::endl;
    
    if (!sideEffects.empty()) {
        std::cout << "│ Side Effects:                           │" << std::endl;
        for (const auto& effect : sideEffects) {
            std::cout << "│   - " << std::setw(36) << effect << "│" << std::endl;
        }
    }
    
    std::cout << "└─────────────────────────────────────────┘" << std::endl;
}

void Medication::writeToDisk(std::ofstream& file) const {
    // Helper lambda for writing strings
    auto writeString = [&](const std::string& str) {
        size_t len = str.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(str.c_str(), len);
    };
    
    // Write strings
    writeString(drugName);
    writeString(genericName);
    writeString(category);
    writeString(dosageForm);
    
    // Write primitive types
    file.write(reinterpret_cast<const char*>(&standardDosage), sizeof(standardDosage));
    file.write(reinterpret_cast<const char*>(&requiresPrescription), sizeof(requiresPrescription));
    
    // Write vectors
    auto writeVector = [&](const std::vector<std::string>& vec) {
        size_t size = vec.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& str : vec) {
            writeString(str);
        }
    };
    
    writeVector(sideEffects);
    writeVector(contraindications);
}

void Medication::readFromDisk(std::ifstream& file) {
    // Helper lambda for reading strings
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
    
    // Read strings
    drugName = readString();
    genericName = readString();
    category = readString();
    dosageForm = readString();
    
    // Read primitive types
    file.read(reinterpret_cast<char*>(&standardDosage), sizeof(standardDosage));
    file.read(reinterpret_cast<char*>(&requiresPrescription), sizeof(requiresPrescription));
    
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
    
    sideEffects = readVector();
    contraindications = readVector();
}