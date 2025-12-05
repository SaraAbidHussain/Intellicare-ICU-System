#include "vital_record.h"
#include <iostream>
#include <iomanip>

void VitalRecord::display() const {
    std::cout << "Patient: " << patientID 
              << " | Time: " << timestamp
              << " | HR: " << heart_rate << " bpm"
              << " | BP: " << systolic_bp << "/" << diastolic_bp
              << " | SpO2: " << spo2 << "%"
              << " | Temp: " << std::fixed << std::setprecision(1) 
              << temperature << "°C" << std::endl;
}

// Write record to disk in binary format
void VitalRecord::writeToDisk(std::ofstream& file) const {
    file.write(reinterpret_cast<const char*>(&patientID), sizeof(patientID));
    file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
    file.write(reinterpret_cast<const char*>(&heart_rate), sizeof(heart_rate));
    file.write(reinterpret_cast<const char*>(&systolic_bp), sizeof(systolic_bp));
    file.write(reinterpret_cast<const char*>(&diastolic_bp), sizeof(diastolic_bp));
    file.write(reinterpret_cast<const char*>(&spo2), sizeof(spo2));
    file.write(reinterpret_cast<const char*>(&temperature), sizeof(temperature));
}

// Read record from disk
void VitalRecord::readFromDisk(std::ifstream& file) {
    file.read(reinterpret_cast<char*>(&patientID), sizeof(patientID));
    file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
    file.read(reinterpret_cast<char*>(&heart_rate), sizeof(heart_rate));
    file.read(reinterpret_cast<char*>(&systolic_bp), sizeof(systolic_bp));
    file.read(reinterpret_cast<char*>(&diastolic_bp), sizeof(diastolic_bp));
    file.read(reinterpret_cast<char*>(&spo2), sizeof(spo2));
    file.read(reinterpret_cast<char*>(&temperature), sizeof(temperature));
}