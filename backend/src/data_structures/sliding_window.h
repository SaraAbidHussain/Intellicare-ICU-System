//file structure: backend/src/data_structures/sliding_window.h
#ifndef SLIDING_WINDOW_H
#define SLIDING_WINDOW_H

#include "circular_buffer.h"
#include "../models/vital_record.h"
#include <map>
#include <vector>

// Sliding window manager for patient vitals monitoring
class SlidingWindow {
private:
    // One circular buffer per patient
    std::map<int, CircularBuffer<VitalRecord>*> patientWindows;
    
    int windowSize;  // Number of readings to keep
    std::string dataFilePath;

public:
    // Constructor & Destructor
    SlidingWindow(int size = 100, const std::string& filePath = "");
    ~SlidingWindow();
    
    // Add new vital reading for a patient
    void addReading(int patientID, const VitalRecord& reading);
    
    // Get last N readings for a patient
    std::vector<VitalRecord> getLastReadings(int patientID, int n);
    
    // Get all readings in window for a patient
    std::vector<VitalRecord> getAllReadings(int patientID);
    
    // Get statistics for a patient's vitals in current window
    struct VitalStats {
        double avgHeartRate;
        double avgSystolicBP;
        double avgDiastolicBP;
        double avgSpO2;
        double avgTemperature;
        
        int minHeartRate;
        int maxHeartRate;
        int minSystolicBP;
        int maxSystolicBP;
        
        int readingCount;
        
        void display() const;
    };
    
    VitalStats getStatistics(int patientID);
    
    // Check if patient has a monitoring window
    bool hasPatient(int patientID) const;
    
    // Get number of patients being monitored
    int getPatientCount() const { return patientWindows.size(); }
    
    // Get all monitored patient IDs
    std::vector<int> getAllPatientIDs() const;
    
    // Clear window for specific patient
    void clearPatient(int patientID);
    
    // Clear all windows
    void clearAll();
    
    // Display all windows
    void display() const;
    void displayPatient(int patientID) const;
    
    // Persistence
    void saveToDisk();
    void loadFromDisk();
};

#endif