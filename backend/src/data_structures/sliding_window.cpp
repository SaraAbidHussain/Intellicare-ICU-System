#include "sliding_window.h"
#include <numeric>
#include <iomanip>

SlidingWindow::SlidingWindow(int size, const std::string& filePath)
    : windowSize(size), dataFilePath(filePath) {
    
    if (!dataFilePath.empty()) {
        loadFromDisk();
    }
    
    std::cout << "[SLIDING-WINDOW] Initialized with window size: " << windowSize << std::endl;
}

SlidingWindow::~SlidingWindow() {
    if (!dataFilePath.empty()) {
        saveToDisk();
    }
    
    // Clean up all circular buffers
    for (auto& pair : patientWindows) {
        delete pair.second;
    }
}

void SlidingWindow::addReading(int patientID, const VitalRecord& reading) {
    // Create buffer for new patient if doesn't exist
    if (patientWindows.find(patientID) == patientWindows.end()) {
        patientWindows[patientID] = new CircularBuffer<VitalRecord>(windowSize, true);
        std::cout << "[SLIDING-WINDOW] Created window for patient " << patientID << std::endl;
    }
    
    patientWindows[patientID]->push(reading);
}

std::vector<VitalRecord> SlidingWindow::getLastReadings(int patientID, int n) {
    if (patientWindows.find(patientID) == patientWindows.end()) {
        return std::vector<VitalRecord>();
    }
    
    return patientWindows[patientID]->getLast(n);
}

std::vector<VitalRecord> SlidingWindow::getAllReadings(int patientID) {
    if (patientWindows.find(patientID) == patientWindows.end()) {
        return std::vector<VitalRecord>();
    }
    
    return patientWindows[patientID]->getAll();
}

SlidingWindow::VitalStats SlidingWindow::getStatistics(int patientID) {
    VitalStats stats;
    stats.readingCount = 0;
    stats.avgHeartRate = 0;
    stats.avgSystolicBP = 0;
    stats.avgDiastolicBP = 0;
    stats.avgSpO2 = 0;
    stats.avgTemperature = 0;
    stats.minHeartRate = 999;
    stats.maxHeartRate = 0;
    stats.minSystolicBP = 999;
    stats.maxSystolicBP = 0;
    
    if (patientWindows.find(patientID) == patientWindows.end()) {
        return stats;
    }
    
    auto readings = patientWindows[patientID]->getAll();
    stats.readingCount = readings.size();
    
    if (readings.empty()) {
        return stats;
    }
    
    // Calculate sums and min/max
    for (const auto& reading : readings) {
        stats.avgHeartRate += reading.heart_rate;
        stats.avgSystolicBP += reading.systolic_bp;
        stats.avgDiastolicBP += reading.diastolic_bp;
        stats.avgSpO2 += reading.spo2;
        stats.avgTemperature += reading.temperature;
        
        if (reading.heart_rate < stats.minHeartRate) stats.minHeartRate = reading.heart_rate;
        if (reading.heart_rate > stats.maxHeartRate) stats.maxHeartRate = reading.heart_rate;
        if (reading.systolic_bp < stats.minSystolicBP) stats.minSystolicBP = reading.systolic_bp;
        if (reading.systolic_bp > stats.maxSystolicBP) stats.maxSystolicBP = reading.systolic_bp;
    }
    
    // Calculate averages
    int count = readings.size();
    stats.avgHeartRate /= count;
    stats.avgSystolicBP /= count;
    stats.avgDiastolicBP /= count;
    stats.avgSpO2 /= count;
    stats.avgTemperature /= count;
    
    return stats;
}

void SlidingWindow::VitalStats::display() const {
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           VITAL SIGNS STATISTICS                   ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\nReadings in window: " << readingCount << "\n" << std::endl;
    
    std::cout << "Heart Rate:" << std::endl;
    std::cout << "  Average: " << std::fixed << std::setprecision(1) << avgHeartRate << " bpm" << std::endl;
    std::cout << "  Range: " << minHeartRate << " - " << maxHeartRate << " bpm\n" << std::endl;
    
    std::cout << "Blood Pressure:" << std::endl;
    std::cout << "  Average: " << (int)avgSystolicBP << "/" << (int)avgDiastolicBP << " mmHg" << std::endl;
    std::cout << "  Systolic Range: " << minSystolicBP << " - " << maxSystolicBP << " mmHg\n" << std::endl;
    
    std::cout << "SpO2: " << std::setprecision(1) << avgSpO2 << "%" << std::endl;
    std::cout << "Temperature: " << std::setprecision(1) << avgTemperature << "°C" << std::endl;
    std::cout << "════════════════════════════════════════════════════\n" << std::endl;
}

bool SlidingWindow::hasPatient(int patientID) const {
    return patientWindows.find(patientID) != patientWindows.end();
}

std::vector<int> SlidingWindow::getAllPatientIDs() const {
    std::vector<int> ids;
    for (const auto& pair : patientWindows) {
        ids.push_back(pair.first);
    }
    return ids;
}

void SlidingWindow::clearPatient(int patientID) {
    auto it = patientWindows.find(patientID);
    if (it != patientWindows.end()) {
        it->second->clear();
        std::cout << "[SLIDING-WINDOW] Cleared window for patient " << patientID << std::endl;
    }
}

void SlidingWindow::clearAll() {
    for (auto& pair : patientWindows) {
        pair.second->clear();
    }
    std::cout << "[SLIDING-WINDOW] Cleared all windows" << std::endl;
}

void SlidingWindow::display() const {
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║        SLIDING WINDOW - ALL PATIENTS               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\nMonitored Patients: " << patientWindows.size() 
              << " | Window Size: " << windowSize << "\n" << std::endl;
    
    for (const auto& pair : patientWindows) {
        std::cout << "Patient " << pair.first << ": " 
                  << pair.second->size() << " readings" << std::endl;
    }
}

void SlidingWindow::displayPatient(int patientID) const {
    auto it = patientWindows.find(patientID);
    if (it == patientWindows.end()) {
        std::cout << "No window found for patient " << patientID << std::endl;
        return;
    }
    
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║      PATIENT " << std::setw(4) << patientID << " - SLIDING WINDOW" 
              << std::string(19, ' ') << "║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    
    auto readings = it->second->getAll();
    std::cout << "\nTotal readings: " << readings.size() << "\n" << std::endl;
    
    for (size_t i = 0; i < readings.size(); i++) {
        std::cout << "[" << i << "] ";
        readings[i].display();
    }
}

void SlidingWindow::saveToDisk() {
    if (dataFilePath.empty()) return;
    
    std::ofstream file(dataFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[SLIDING-WINDOW] Error: Cannot open file for writing" << std::endl;
        return;
    }
    
    // Write window size and patient count
    file.write(reinterpret_cast<const char*>(&windowSize), sizeof(windowSize));
    int patientCount = patientWindows.size();
    file.write(reinterpret_cast<const char*>(&patientCount), sizeof(patientCount));
    
    // Write each patient's data
    for (const auto& pair : patientWindows) {
        int patientID = pair.first;
        file.write(reinterpret_cast<const char*>(&patientID), sizeof(patientID));
        
        auto readings = pair.second->getAll();
        int readingCount = readings.size();
        file.write(reinterpret_cast<const char*>(&readingCount), sizeof(readingCount));
        
        for (const auto& reading : readings) {
            reading.writeToDisk(file);
        }
    }
    
    file.close();
    std::cout << "[SLIDING-WINDOW] Saved windows for " << patientCount 
              << " patients to " << dataFilePath << std::endl;
}

void SlidingWindow::loadFromDisk() {
    if (dataFilePath.empty()) return;
    
    std::ifstream file(dataFilePath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "[SLIDING-WINDOW] No existing data file found" << std::endl;
        return;
    }
    
    // Clear existing data
    clearAll();
    for (auto& pair : patientWindows) {
        delete pair.second;
    }
    patientWindows.clear();
    
    // Read window size and patient count
    file.read(reinterpret_cast<char*>(&windowSize), sizeof(windowSize));
    int patientCount;
    file.read(reinterpret_cast<char*>(&patientCount), sizeof(patientCount));
    
    // Read each patient's data
    for (int i = 0; i < patientCount; i++) {
        int patientID;
        file.read(reinterpret_cast<char*>(&patientID), sizeof(patientID));
        
        int readingCount;
        file.read(reinterpret_cast<char*>(&readingCount), sizeof(readingCount));
        
        patientWindows[patientID] = new CircularBuffer<VitalRecord>(windowSize, true);
        
        for (int j = 0; j < readingCount; j++) {
            VitalRecord reading;
            reading.readFromDisk(file);
            patientWindows[patientID]->push(reading);
        }
    }
    
    file.close();
    std::cout << "[SLIDING-WINDOW] Loaded windows for " << patientCount 
              << " patients from " << dataFilePath << std::endl;
}