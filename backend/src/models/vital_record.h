#ifndef VITAL_RECORD_H
#define VITAL_RECORD_H

#include <string>
#include <ctime>
#include <fstream>

struct VitalRecord {
    int patientID;
    long timestamp;
    int heart_rate;
    int systolic_bp;
    int diastolic_bp;
    int spo2;
    float temperature;
    
    VitalRecord() : patientID(0), timestamp(0), heart_rate(0), 
                    systolic_bp(0), diastolic_bp(0), spo2(0), temperature(0.0) {}
    
    VitalRecord(int pid, long ts, int hr, int sbp, int dbp, int sp, float temp)
        : patientID(pid), timestamp(ts), heart_rate(hr), 
          systolic_bp(sbp), diastolic_bp(dbp), spo2(sp), temperature(temp) {}
    
    void display() const;
    
    // Disk I/O methods
    void writeToDisk(std::ofstream& file) const;
    void readFromDisk(std::ifstream& file);
};

#endif