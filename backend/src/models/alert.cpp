#include "alert.h"
#include <iostream>
#include <iomanip>

Alert::Alert() 
    : alertID(0), patientID(0), priority(INFO), type(CUSTOM),
      message(""), timestamp(0), acknowledged(false), 
      acknowledgedBy(""), acknowledgedTime(0) {}

Alert::Alert(int id, int pid, AlertPriority prio, AlertType t, const std::string& msg)
    : alertID(id), patientID(pid), priority(prio), type(t),
      message(msg), acknowledged(false), acknowledgedBy(""), acknowledgedTime(0) {
    timestamp = time(nullptr);
}

std::string Alert::getPriorityString() const {
    switch(priority) {
        case CRITICAL: return "🔴 CRITICAL";
        case HIGH:     return "🟠 HIGH";
        case MEDIUM:   return "🟡 MEDIUM";
        case LOW:      return "🟢 LOW";
        case INFO:     return "🔵 INFO";
        default:       return "UNKNOWN";
    }
}

std::string Alert::getTypeString() const {
    switch(type) {
        case VITAL_ABNORMAL:    return "Vital Signs Abnormal";
        case DRUG_INTERACTION:  return "Drug Interaction";
        case EQUIPMENT_FAILURE: return "Equipment Failure";
        case DETERIORATION:     return "Patient Deterioration";
        case MEDICATION_DUE:    return "Medication Due";
        case LAB_CRITICAL:      return "Critical Lab Result";
        case CUSTOM:            return "Custom Alert";
        default:                return "Unknown";
    }
}

void Alert::display() const {
    std::cout << "┌─────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ Alert ID: " << std::setw(4) << alertID 
              << " | Patient: " << std::setw(4) << patientID 
              << " | " << getPriorityString() << std::string(10, ' ') << "│" << std::endl;
    std::cout << "│ Type: " << std::left << std::setw(44) << getTypeString() << "│" << std::endl;
    std::cout << "│ Message: " << std::setw(41) << message << "│" << std::endl;
    
    // Convert timestamp to readable format
    char timeStr[80];
    struct tm* timeinfo = localtime(&timestamp);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::cout << "│ Time: " << std::setw(44) << timeStr << "│" << std::endl;
    
    if (acknowledged) {
        std::cout << "│ ✅ Acknowledged by: " << std::setw(29) << acknowledgedBy << "│" << std::endl;
    } else {
        std::cout << "│ ⚠️  NOT ACKNOWLEDGED" << std::string(29, ' ') << "│" << std::endl;
    }
    
    std::cout << "└─────────────────────────────────────────────────────┘" << std::endl;
}

void Alert::writeToDisk(std::ofstream& file) const {
    // Write primitive types
    file.write(reinterpret_cast<const char*>(&alertID), sizeof(alertID));
    file.write(reinterpret_cast<const char*>(&patientID), sizeof(patientID));
    file.write(reinterpret_cast<const char*>(&priority), sizeof(priority));
    file.write(reinterpret_cast<const char*>(&type), sizeof(type));
    file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
    file.write(reinterpret_cast<const char*>(&acknowledged), sizeof(acknowledged));
    file.write(reinterpret_cast<const char*>(&acknowledgedTime), sizeof(acknowledgedTime));
    
    // Write strings
    auto writeString = [&](const std::string& str) {
        size_t len = str.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(str.c_str(), len);
    };
    
    writeString(message);
    writeString(acknowledgedBy);
}

void Alert::readFromDisk(std::ifstream& file) {
    // Read primitive types
    file.read(reinterpret_cast<char*>(&alertID), sizeof(alertID));
    file.read(reinterpret_cast<char*>(&patientID), sizeof(patientID));
    file.read(reinterpret_cast<char*>(&priority), sizeof(priority));
    file.read(reinterpret_cast<char*>(&type), sizeof(type));
    file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
    file.read(reinterpret_cast<char*>(&acknowledged), sizeof(acknowledged));
    file.read(reinterpret_cast<char*>(&acknowledgedTime), sizeof(acknowledgedTime));
    
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
    
    message = readString();
    acknowledgedBy = readString();
}

// Comparison operators for min-heap
// Lower priority number = higher urgency = should come first
bool Alert::operator<(const Alert& other) const {
    // Primary: Compare by priority (lower number = higher priority)
    if (priority != other.priority) {
        return priority < other.priority;
    }
    // Secondary: If same priority, older alert first
    return timestamp < other.timestamp;
}

bool Alert::operator>(const Alert& other) const {
    if (priority != other.priority) {
        return priority > other.priority;
    }
    return timestamp > other.timestamp;
}