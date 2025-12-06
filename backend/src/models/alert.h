#ifndef ALERT_H
#define ALERT_H

#include <string>
#include <ctime>
#include <fstream>
#include <iostream>

// Alert priority levels
enum AlertPriority {
    CRITICAL = 1,    // Immediate action required
    HIGH = 2,        // Urgent attention needed
    MEDIUM = 3,      // Should be addressed soon
    LOW = 4,         // For information
    INFO = 5         // General notification
};

// Alert types
enum AlertType {
    VITAL_ABNORMAL,      // Vital signs out of range
    DRUG_INTERACTION,    // Dangerous drug combination
    EQUIPMENT_FAILURE,   // Monitor/equipment issue
    DETERIORATION,       // Patient condition worsening
    MEDICATION_DUE,      // Time to administer medication
    LAB_CRITICAL,        // Critical lab result
    CUSTOM               // Custom alert
};

struct Alert {
    int alertID;
    int patientID;
    AlertPriority priority;
    AlertType type;
    std::string message;
    long timestamp;          // When alert was created
    bool acknowledged;       // Has someone seen this?
    std::string acknowledgedBy;
    long acknowledgedTime;
    
    // Constructors
    Alert();
    Alert(int id, int pid, AlertPriority prio, AlertType t, const std::string& msg);
    
    // Display
    void display() const;
    std::string getPriorityString() const;
    std::string getTypeString() const;
    
    // Disk I/O
    void writeToDisk(std::ofstream& file) const;
    void readFromDisk(std::ifstream& file);
    
    // Comparison operators for heap operations
    bool operator<(const Alert& other) const;
    bool operator>(const Alert& other) const;
};

#endif