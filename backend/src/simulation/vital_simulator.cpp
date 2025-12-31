#include "vital_simulator.h"
#include <chrono>
#include <iostream>

VitalSimulator::VitalSimulator() : gen(rd()) {
    std::cout << "[SIMULATOR] Initialized" << std::endl;
}

VitalSimulator::~VitalSimulator() {
    stopAll();
}

void VitalSimulator::setCallback(std::function<void(const VitalRecord&)> callback) {
    onVitalGenerated = callback;
}

bool VitalSimulator::startSimulation(int patientID, const VitalRecord& baseVitals, int intervalMs) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Stop existing simulation if running
    if (activeSimulations.find(patientID) != activeSimulations.end()) {
        std::cout << "[SIMULATOR] Stopping existing simulation for patient " << patientID << std::endl;
        stopSimulation(patientID);
    }
    
    // Create config
    SimulationConfig config;
    config.patientID = patientID;
    config.intervalMs = intervalMs;
    config.enabled = true;
    config.activeEvent = NORMAL;
    config.baseHeartRate = baseVitals.heart_rate;
    config.baseSystolicBP = baseVitals.systolic_bp;
    config.baseDiastolicBP = baseVitals.diastolic_bp;
    config.baseSpO2 = baseVitals.spo2;
    config.baseTemperature = baseVitals.temperature;
    
    activeSimulations[patientID] = config;
    
    // Start simulation thread
    simulationThreads[patientID] = std::thread(&VitalSimulator::simulatePatient, this, patientID);
    
    std::cout << "[SIMULATOR] Started simulation for patient " << patientID 
              << " (interval: " << intervalMs << "ms)" << std::endl;
    
    return true;
}

bool VitalSimulator::stopSimulation(int patientID) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = activeSimulations.find(patientID);
    if (it == activeSimulations.end()) {
        return false;
    }
    
    // Mark as disabled
    it->second.enabled = false;
    
    // Wait for thread to finish
    if (simulationThreads.find(patientID) != simulationThreads.end()) {
        if (simulationThreads[patientID].joinable()) {
            simulationThreads[patientID].join();
        }
        simulationThreads.erase(patientID);
    }
    
    activeSimulations.erase(patientID);
    
    std::cout << "[SIMULATOR] Stopped simulation for patient " << patientID << std::endl;
    
    return true;
}

bool VitalSimulator::isRunning(int patientID) {
    std::lock_guard<std::mutex> lock(mutex);
    return activeSimulations.find(patientID) != activeSimulations.end();
}

bool VitalSimulator::triggerEvent(int patientID, SimulationEvent event) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = activeSimulations.find(patientID);
    if (it == activeSimulations.end()) {
        return false;
    }
    
    it->second.activeEvent = event;
    
    std::cout << "[SIMULATOR] Triggered event " << event 
              << " for patient " << patientID << std::endl;
    
    return true;
}

SimulationConfig VitalSimulator::getConfig(int patientID) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = activeSimulations.find(patientID);
    if (it != activeSimulations.end()) {
        return it->second;
    }
    
    return SimulationConfig();
}

void VitalSimulator::stopAll() {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::cout << "[SIMULATOR] Stopping all simulations..." << std::endl;
    
    // Mark all as disabled
    for (auto& pair : activeSimulations) {
        pair.second.enabled = false;
    }
    
    // Wait for all threads
    for (auto& pair : simulationThreads) {
        if (pair.second.joinable()) {
            pair.second.join();
        }
    }
    
    activeSimulations.clear();
    simulationThreads.clear();
    
    std::cout << "[SIMULATOR] All simulations stopped" << std::endl;
}

std::vector<int> VitalSimulator::getActiveSimulations() {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<int> active;
    for (const auto& pair : activeSimulations) {
        active.push_back(pair.first);
    }
    
    return active;
}

void VitalSimulator::simulatePatient(int patientID) {
    while (true) {
        SimulationConfig config;
        
        {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = activeSimulations.find(patientID);
            
            if (it == activeSimulations.end() || !it->second.enabled) {
                break;
            }
            
            config = it->second;
        }
        
        // Generate vital signs
        VitalRecord vitals = generateVitals(config);
        
        // Call callback if set
        if (onVitalGenerated) {
            onVitalGenerated(vitals);
        }
        
        // Sleep for interval
        std::this_thread::sleep_for(std::chrono::milliseconds(config.intervalMs));
    }
}

VitalRecord VitalSimulator::generateVitals(const SimulationConfig& config) {
    VitalRecord vitals;
    vitals.patientID = config.patientID;
    vitals.timestamp = time(nullptr);
    
    switch (config.activeEvent) {
        case NORMAL:
            vitals.heart_rate = addVariation(config.baseHeartRate, -3, 3);
            vitals.systolic_bp = addVariation(config.baseSystolicBP, -5, 5);
            vitals.diastolic_bp = addVariation(config.baseDiastolicBP, -3, 3);
            vitals.spo2 = addVariation(config.baseSpO2, -1, 1);
            vitals.temperature = addVariation(config.baseTemperature, -0.2f, 0.2f);
            break;
            
        case BRADYCARDIA:
            vitals.heart_rate = addVariation(50, -5, 5);
            vitals.systolic_bp = addVariation(config.baseSystolicBP, -5, 5);
            vitals.diastolic_bp = addVariation(config.baseDiastolicBP, -3, 3);
            vitals.spo2 = addVariation(config.baseSpO2, -1, 1);
            vitals.temperature = addVariation(config.baseTemperature, -0.2f, 0.2f);
            break;
            
        case TACHYCARDIA:
            vitals.heart_rate = addVariation(130, -5, 10);
            vitals.systolic_bp = addVariation(config.baseSystolicBP + 10, -5, 5);
            vitals.diastolic_bp = addVariation(config.baseDiastolicBP + 5, -3, 3);
            vitals.spo2 = addVariation(config.baseSpO2, -2, 0);
            vitals.temperature = addVariation(config.baseTemperature, -0.2f, 0.2f);
            break;
            
        case HYPOTENSION:
            vitals.heart_rate = addVariation(config.baseHeartRate + 10, -5, 5);
            vitals.systolic_bp = addVariation(85, -5, 5);
            vitals.diastolic_bp = addVariation(55, -3, 3);
            vitals.spo2 = addVariation(config.baseSpO2 - 2, -1, 1);
            vitals.temperature = addVariation(config.baseTemperature, -0.2f, 0.2f);
            break;
            
        case HYPERTENSION:
            vitals.heart_rate = addVariation(config.baseHeartRate, -3, 3);
            vitals.systolic_bp = addVariation(160, -5, 10);
            vitals.diastolic_bp = addVariation(100, -3, 5);
            vitals.spo2 = addVariation(config.baseSpO2, -1, 1);
            vitals.temperature = addVariation(config.baseTemperature, -0.2f, 0.2f);
            break;
            
        case HYPOXIA:
            vitals.heart_rate = addVariation(config.baseHeartRate + 15, -5, 5);
            vitals.systolic_bp = addVariation(config.baseSystolicBP, -5, 5);
            vitals.diastolic_bp = addVariation(config.baseDiastolicBP, -3, 3);
            vitals.spo2 = addVariation(88, -2, 2);
            vitals.temperature = addVariation(config.baseTemperature, -0.2f, 0.2f);
            break;
            
        case FEVER:
            vitals.heart_rate = addVariation(config.baseHeartRate + 10, -3, 5);
            vitals.systolic_bp = addVariation(config.baseSystolicBP, -5, 5);
            vitals.diastolic_bp = addVariation(config.baseDiastolicBP, -3, 3);
            vitals.spo2 = addVariation(config.baseSpO2, -1, 1);
            vitals.temperature = addVariation(38.5f, -0.3f, 0.5f);
            break;
            
        case HYPOTHERMIA:
            vitals.heart_rate = addVariation(config.baseHeartRate - 10, -5, 5);
            vitals.systolic_bp = addVariation(config.baseSystolicBP - 10, -5, 5);
            vitals.diastolic_bp = addVariation(config.baseDiastolicBP - 5, -3, 3);
            vitals.spo2 = addVariation(config.baseSpO2, -1, 1);
            vitals.temperature = addVariation(35.5f, -0.3f, 0.3f);
            break;
    }
    
    // Ensure values stay within realistic bounds
    vitals.heart_rate = std::max(30, std::min(200, vitals.heart_rate));
    vitals.systolic_bp = std::max(60, std::min(200, vitals.systolic_bp));
    vitals.diastolic_bp = std::max(40, std::min(130, vitals.diastolic_bp));
    vitals.spo2 = std::max(70, std::min(100, vitals.spo2));
    vitals.temperature = std::max(35.0f, std::min(42.0f, vitals.temperature));
    
    return vitals;
}

int VitalSimulator::addVariation(int base, int min, int max) {
    std::uniform_int_distribution<> dis(min, max);
    return base + dis(gen);
}

float VitalSimulator::addVariation(float base, float min, float max) {
    std::uniform_real_distribution<> dis(min, max);
    return base + dis(gen);
}