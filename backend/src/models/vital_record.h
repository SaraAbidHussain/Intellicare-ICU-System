#ifndef VITAL_RECORD_H
#define VITAL_RECORD_H

#include <string>
#include <ctime>
#include <fstream>

// Fixed-size record for disk storage (no dynamic allocation)
struct VitalRecord {
    int patientID;
    long timestamp;
    int heart_rate;
    int systolic_bp;
    int diastolic_bp;
    int spo2;
    float temperature;
    
    // Position in data file (-1 if not saved)
    long diskPosition;
    
    VitalRecord();
    VitalRecord(int pid, long ts, int hr, int sbp, int dbp, int sp, float temp);
    
    void display() const;
    
    // Fixed-size disk I/O
    void writeToDisk(std::ofstream& file) const;
    void readFromDisk(std::ifstream& file);
    
    // Get fixed size for disk storage
    static size_t getDiskSize();
};

#endif