//file structure: backend/src/analysis/vital_analyzer.h
#ifndef VITAL_ANALYZER_H
#define VITAL_ANALYZER_H

#include <vector>
#include <string>
#include "../models/vital_record.h"

// Pattern types
enum PatternType {
    PATTERN_NONE,
    SUDDEN_SPIKE,       // >20% increase in short time
    SUDDEN_DROP,        // >20% decrease in short time
    SUSTAINED_HIGH,     // Above threshold for extended period
    SUSTAINED_LOW,      // Below threshold for extended period
    ERRATIC,            // High variability/instability
    TRENDING_UP,        // Gradual increase over time
    TRENDING_DOWN       // Gradual decrease over time
};

// Detected pattern
struct DetectedPattern {
    PatternType type;
    std::string vitalSign;  // Which vital (HR, BP, SpO2, Temp)
    std::string description;
    float changeValue;      // Amount of change
    int timeWindowMinutes;  // Over what time period
    bool isCritical;        // Requires immediate attention
    
    DetectedPattern() 
        : type(PATTERN_NONE), vitalSign(""), description(""), 
          changeValue(0), timeWindowMinutes(0), isCritical(false) {}
};

// Risk factors
struct RiskFactors {
    int abnormalVitalsCount;
    int criticalAlertsCount;
    int drugInteractionSeverity;
    bool hasDeteriorationTrend;
    bool hasErraticPattern;
    
    RiskFactors() 
        : abnormalVitalsCount(0), criticalAlertsCount(0), 
          drugInteractionSeverity(0), hasDeteriorationTrend(false),
          hasErraticPattern(false) {}
};

// Risk assessment report
struct RiskAssessment {
    int riskScore;           // 0-100 (0=low, 100=critical)
    std::string riskLevel;   // "LOW", "MEDIUM", "HIGH", "CRITICAL"
    RiskFactors factors;
    std::vector<std::string> recommendations;
    std::vector<DetectedPattern> patterns;
    
    RiskAssessment() : riskScore(0), riskLevel("LOW") {}
};

class VitalAnalyzer {
private:
    // Check if vital is within normal range
    bool isHeartRateNormal(int hr);
    bool isBloodPressureNormal(int systolic, int diastolic);
    bool isSpO2Normal(int spo2);
    bool isTemperatureNormal(float temp);
    
    // Calculate percentage change
    float calculatePercentChange(float oldVal, float newVal);
    
    // Detect specific patterns
    DetectedPattern detectSuddenChange(const std::vector<VitalRecord>& readings, 
                                      const std::string& vitalSign);
    DetectedPattern detectTrend(const std::vector<VitalRecord>& readings,
                               const std::string& vitalSign);
    DetectedPattern detectErratic(const std::vector<VitalRecord>& readings,
                                 const std::string& vitalSign);
    
    // Calculate variability (standard deviation)
    float calculateVariability(const std::vector<float>& values);
    
public:
    VitalAnalyzer();
    
    // Analyze patterns in vital sign history
    std::vector<DetectedPattern> analyzePatterns(const std::vector<VitalRecord>& readings);
    
    // Calculate risk score for a patient
    RiskAssessment calculateRiskScore(const std::vector<VitalRecord>& recentReadings,
                                     int alertCount,
                                     int drugInteractionSeverity);
    
    // Check if current vitals are abnormal
    bool checkAbnormal(const VitalRecord& vitals, std::vector<std::string>& abnormalities);
    
    // Quick status check
    std::string getVitalStatus(const VitalRecord& vitals);
};

#endif