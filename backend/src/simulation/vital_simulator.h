//file structure: backend/src/simulation/vital_simulator.h
#ifndef VITAL_SIMULATOR_H
#define VITAL_SIMULATOR_H

#include <thread>
#include <atomic>
#include <random>
#include <functional>
#include <map>
#include <vector>
#include <mutex>
#include "../models/vital_record.h"

// Simulation event types
enum SimulationEvent {
    NORMAL,           // Normal variations
    BRADYCARDIA,      // Low heart rate
    TACHYCARDIA,      // High heart rate
    HYPOTENSION,      // Low blood pressure
    HYPERTENSION,     // High blood pressure
    HYPOXIA,          // Low oxygen
    FEVER,            // High temperature
    HYPOTHERMIA       // Low temperature
};

// Simulation configuration
struct SimulationConfig {
    int patientID;
    int intervalMs;        // Update interval in milliseconds (default: 2000 = 2 seconds)
    bool enabled;
    SimulationEvent activeEvent;
    
    // Base vitals (starting point)
    int baseHeartRate;
    int baseSystolicBP;
    int baseDiastolicBP;
    int baseSpO2;
    float baseTemperature;
    
    SimulationConfig() 
        : patientID(0), intervalMs(2000), enabled(false), 
          activeEvent(NORMAL),
          baseHeartRate(75), baseSystolicBP(120), baseDiastolicBP(80),
          baseSpO2(98), baseTemperature(37.0f) {}
};

class VitalSimulator {
private:
    std::map<int, SimulationConfig> activeSimulations;
    std::map<int, std::thread> simulationThreads;
    std::mutex mutex;
    
    // Random number generation
    std::random_device rd;
    std::mt19937 gen;
    
    // Callback function for generated vitals
    std::function<void(const VitalRecord&)> onVitalGenerated;
    
    // Simulation thread function
    void simulatePatient(int patientID);
    
    // Generate variations based on event type
    VitalRecord generateVitals(const SimulationConfig& config);
    int addVariation(int base, int min, int max);
    float addVariation(float base, float min, float max);
    
public:
    VitalSimulator();
    ~VitalSimulator();
    
    // Set callback for when vitals are generated
    void setCallback(std::function<void(const VitalRecord&)> callback);
    
    // Start simulation for a patient
    bool startSimulation(int patientID, const VitalRecord& baseVitals, int intervalMs = 2000);
    
    // Stop simulation for a patient
    bool stopSimulation(int patientID);
    
    // Check if simulation is running
    bool isRunning(int patientID);
    
    // Trigger an abnormal event
    bool triggerEvent(int patientID, SimulationEvent event);
    
    // Get current simulation config
    SimulationConfig getConfig(int patientID);
    
    // Stop all simulations
    void stopAll();
    
    // Get list of active simulations
    std::vector<int> getActiveSimulations();
};

#endif